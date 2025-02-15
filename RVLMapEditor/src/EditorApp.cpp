#include "EditorApp.hpp"
#include "MapEditorState.hpp"

#include <RVL/Core/Utils/Files.hpp>

#include <nfd.h>

namespace Rvl
{ 
    struct AppUIData
    {
        bool ProjectCreation, ProjectOpen, PrjCreateWindowOpen = true;
        bool DeleteFileWindow = false;
        bool DeleteTileSet = false, DeleteTileMap = true;
        bool UseTls = false, UseTlm = false, UseTexture = true;
        bool TextureSelected = false;

        static constexpr size_t MaxNameSize = 100;
        char ProjectName[MaxNameSize];

        std::string ErrorText;

        std::string TexName;
        std::string TlmName;
        std::string TlsName;

        ImGuiWindowFlags ProjectWinFlags;

        std::string DeletingTlsPath, DeletingTlmPath, DeletingProjectName;
    };

    static AppUIData UIData;

    EditorApp::EditorApp() : App(1000, 700, "RVL Map Editor") {}
    EditorApp::~EditorApp() {}

    void EditorApp::Start()
    {
        SetClearColor({0.0f, 0.3f, 0.3f});

        _prjfileText = Utils::GetTextFromFile("./settings/projects.rconfig");

        if (_prjfileText != "")
        {
            UIData.ProjectCreation = false;
            UIData.ProjectOpen = true;
        }
        else
        {
            UIData.ProjectCreation = true;
            UIData.ProjectOpen = false;
        }

        SetDefaults();

        try
        {
            _parser = ConfigParser("settings/projects.rconfig");
        }
        catch (Error err)
        {
            if (err.Status == RVL_CLIENT_ERROR)
                err.Print();
            else
                throw err;
        }
    }

    void EditorApp::Update()
    {
        DockspaceAndMenu();

        if (UIData.ProjectCreation)
            ProjectWindow();

        if (UIData.ProjectOpen)
            ProjectOpenWindow();
        else
            UIData.ProjectWinFlags &= ~ImGuiWindowFlags_NoInputs;


        if (!UIData.PrjCreateWindowOpen)
        {
            UIData.ProjectCreation = false;
            if (_currentState)
                dynamic_cast<MapEditorState*>(_currentState.get())->InputsEnabled(true);
        }

        if (!UIData.ProjectCreation && !UIData.ProjectOpen && _currentState)
        {
            _currentState->Update();
        }
    }

    void EditorApp::DockspaceAndMenu()
    {
        static bool dockspaceOpen = true;
        static bool optFullscreen = true;
        static bool optPadding = false;

        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (optFullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            windowFlags |= ImGuiWindowFlags_NoBackground;

        if (!optPadding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
        if (!optPadding)
            ImGui::PopStyleVar();

        if (optFullscreen)
            ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Project", nullptr, false))
                {
                    UIData.ProjectCreation = true;
                    UIData.PrjCreateWindowOpen = true;
                    SetDefaults();
                }                 
                if (ImGui::MenuItem("Open Project", nullptr, false))
                {
                    UIData.ProjectOpen = true;
                    SetDefaults();
                } 
                if (ImGui::MenuItem("Save", nullptr, false,  _currentState != nullptr))
                    dynamic_cast<MapEditorState*>(_currentState.get())->Save();

                if (ImGui::MenuItem("Exit", nullptr, false)) Close();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", nullptr, false, _currentState != nullptr))
                    dynamic_cast<MapEditorState*>(_currentState.get())->Undo();

                if (ImGui::MenuItem("Redo", nullptr, false, _currentState != nullptr))
                    dynamic_cast<MapEditorState*>(_currentState.get())->Redo();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void EditorApp::ProjectWindow()
    {

        if (_currentState)
            dynamic_cast<MapEditorState*>(_currentState.get())->InputsEnabled(false);

        ImGui::Begin("Project", &UIData.PrjCreateWindowOpen, UIData.ProjectWinFlags);
        ImGui::Text("Create New Project: \n\n");

        ImGui::Text("Project name (max 100 symbols)");
        ImGui::InputText("##prjNameInput", UIData.ProjectName, UIData.MaxNameSize);
        ImGui::Text("\n");

        if (ImGui::RadioButton("Texture atlas", UIData.UseTexture))
        {
            UIData.UseTexture = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Tileset/map", !UIData.UseTexture))
        {
            UIData.UseTexture = false;
        }

        if (UIData.UseTexture)
        {
            if (ImGui::Button("Select texture ..."))
            {
                nfdchar_t* openPath = nullptr;
                nfdresult_t result = NFD_OpenDialog("png,jpg", "./", &openPath);

                if ( result == NFD_OKAY )
                {
                    UIData.TexName = openPath;
                    UIData.TextureSelected = true;
                    free(openPath);
                }
                else if (result == NFD_ERROR)
                {
                    UIData.ErrorText = "Error when selecting a file";
                }   
            }

            if (UIData.TextureSelected)
            {
                ImGui::SameLine();
                
                auto nameTokens = Utils::SplitStr(UIData.TexName, '/');
                ImGui::Text("%s", nameTokens.back().c_str());
                ImGui::SameLine();

                if (ImGui::Button("Clear", ImVec2(40, 20)))
                {
                    UIData.TexName = "";
                    UIData.TextureSelected = false;
                }
            }
        }
        else
        {
            if (ImGui::Button("Select tileset ..."))
            {
                nfdchar_t* openPath = nullptr;
                nfdresult_t result = NFD_OpenDialog("rtls", "./", &openPath);

                if ( result == NFD_OKAY )
                {
                    UIData.UseTls = true;
                    UIData.TlsName = openPath;
                    free(openPath);
                }
                else if (result == NFD_ERROR)
                {
                    UIData.ErrorText = "Error when selecting a file";
                }
            }

            if (UIData.UseTls)
            {
                ImGui::SameLine();
                
                auto nameTokens = Utils::SplitStr(UIData.TlsName, '/');
                ImGui::Text("%s", nameTokens.back().c_str());
                ImGui::SameLine();

                if (ImGui::Button("Clear", ImVec2(40, 20)))
                {
                    UIData.TlsName = "";
                    UIData.UseTls = false;
                }
            }

            if (ImGui::Button("Select tilemap ..."))
            {
                nfdchar_t* openPath = nullptr;
                nfdresult_t result = NFD_OpenDialog("rtlm", "./", &openPath);

                if ( result == NFD_OKAY )
                {
                    UIData.UseTlm = true;
                    UIData.TlmName = openPath;
                    RVL_LOG(UIData.TlmName);
                    free(openPath);
                }
                else if (result == NFD_ERROR)
                {
                    UIData.ErrorText = "Error when selecting a file";
                }
            }

            if (UIData.UseTlm)
            {
                ImGui::SameLine();
                
                auto nameTokens = Utils::SplitStr(UIData.TlmName, '/');
                ImGui::Text("%s", nameTokens.back().c_str());
                ImGui::SameLine();

                if (ImGui::Button("Clear", ImVec2(40, 20)))
                {
                    UIData.TlmName = "";
                    UIData.UseTlm = false;
                }
            }
        }

        if (ImGui::Button("Create Project", ImVec2(ImGui::GetContentRegionAvail().x, 40)))
        {
            std::regex nameRegex (R"(^[_|A-Z|a-z]+[_|A-Z|a-z|0-9]+)");

            if (!std::regex_match(UIData.ProjectName, nameRegex))
            {
                UIData.ErrorText = "Project name is invalid";
            }
            else
            {
                if (!UIData.UseTls && !UIData.UseTlm)
                {
                    if (!UIData.TextureSelected)
                    {    
                        UIData.ErrorText = "Texture isn't selected";
                    }
                    else
                    {
                        _currentState = NewPtr<MapEditorState>(UIData.ProjectName, UIData.TexName);
                        _currentState->Start();
                        UIData.ErrorText.clear();
                    }
                }
                else if (UIData.UseTls && !UIData.UseTlm)
                {
                    Ref<TileSet> tls = NewRef<TileSet>(UIData.TlsName);
                    _currentState = NewPtr<MapEditorState>(UIData.ProjectName, tls);
                    _currentState->Start();
                    UIData.ErrorText.clear();
                }
                else if (UIData.UseTls && UIData.UseTlm)
                {
                    Ref<TileSet> tls = NewRef<TileSet>(UIData.TlsName);
                    _currentState = NewPtr<MapEditorState>(UIData.ProjectName, tls, UIData.TlmName, 2, 0.01f);
                    _currentState->Start();
                    UIData.ErrorText.clear();
                }
                else
                    UIData.ErrorText = "Tileset isn't selected";
            }
            
            if (UIData.ErrorText.empty())
                UIData.ProjectCreation = false;
        }

        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.f), "%s", UIData.ErrorText.c_str());

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 100);
        ImGui::Separator();
        ImGui::Text("Or: \n");

        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(232, 108, 108, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 128, 128, 255));
        if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x / 2 - 10, 30)))
        {
            UIData.ProjectCreation = false;

            if (_currentState)
                dynamic_cast<MapEditorState*>(_currentState.get())->InputsEnabled(true);
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::SameLine();

        if (ImGui::Button("Open Existing", ImVec2(ImGui::GetContentRegionAvail().x - 10, 30)))
        {
            UIData.ProjectWinFlags |= ImGuiWindowFlags_NoInputs;
            UIData.ProjectOpen = true;
        }

        ImGui::End();
    }

    void EditorApp::ProjectOpenWindow()
    {
        ImGui::Begin("Projects", &UIData.ProjectOpen,  ImGuiWindowFlags_NoDocking);

        _parser.Parse();

        auto arrs = _parser.GetStringArrays();

        int i = 0;

        for (auto& arr : arrs)
        {
            i++;
            auto last2 = Utils::SplitStr(arr.second[0], '/').back();
            auto last1 = Utils::SplitStr(arr.second[1], '/').back();

            std::string project = std::to_string(i) + ". " + arr.first + " (" + last2 + " " + last1 + ")";
            ImGui::Text("%s", project.c_str());
            ImGui::SameLine();
            ImGui::PushID(i);
            if (ImGui::Button("Open", ImVec2(50, 20)))
            {
                _currentState = NewPtr<MapEditorState>(arr.first);
                _currentState->Start();
                UIData.ProjectOpen = false;
                UIData.ProjectCreation = false;
            }
            ImGui::PopID();

            ImGui::PushID(i + 1);
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(232, 94, 94, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 108, 108, 255));
            if (ImGui::Button("Delete", ImVec2(50, 20)))
            {
                UIData.DeletingProjectName = arr.first;
                UIData.DeletingTlsPath = arr.second[0];
                UIData.DeletingTlmPath = arr.second[1];
                UIData.DeleteFileWindow = true;
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopID();
        }

        if (ImGui::Button("Create New", ImVec2(100, 25)))
        {
            UIData.ProjectWinFlags &= ~ImGuiWindowFlags_NoInputs;
            UIData.ProjectOpen = false;
            UIData.ProjectCreation = true;
        }

        ImGui::End();

        if (UIData.DeleteFileWindow)
        {
            ImGui::Begin("Delete Project", &UIData.DeleteFileWindow, ImGuiWindowFlags_NoDocking);
            ImGui::Checkbox("Delete tile set", &UIData.DeleteTileSet);
            ImGui::Checkbox("Delete tile map", &UIData.DeleteTileMap);

            if (ImGui::Button("OK", ImVec2(50, 20)))
            {
                if (_currentState)
                {
                    if (dynamic_cast<MapEditorState*>(_currentState.get())->GetProjectName() == UIData.DeletingProjectName)
                    {
                        MapEditorState* state = dynamic_cast<MapEditorState*>(_currentState.release());
                        delete state;
                    }
                }

                if (UIData.DeleteTileSet)
                    std::filesystem::remove(UIData.DeletingTlsPath);

                if (UIData.DeleteTileMap)
                    std::filesystem::remove(UIData.DeletingTlmPath);

                _parser.Delete(UIData.DeletingProjectName);
                _parser.Save();

                UIData.DeletingTlsPath = "";
                UIData.DeletingTlmPath = "";
                UIData.DeletingProjectName = "";

                UIData.DeleteFileWindow = false;
            }
            ImGui::End();
        }
    }
    
    void EditorApp::SetDefaults()
    {
        UIData.ProjectName[0] = '\0';

        UIData.DeleteTileMap = true;
        UIData.DeleteTileSet = false;

        UIData.ProjectWinFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking;
    }
}
