#pragma once

#include "Core/UI/HierarchyWindow.hpp"
#include "Core/UI/InspectorWindow.hpp"
#include "2D/ParticleEmitter.hpp"

using namespace Rvl;

class EditorState : public Rvl::State
{
public:
    EditorState();
    ~EditorState();

    void Start() override;
    void Update() override;
    void Render() override;
    void PostRender() override;

private:
    void RenderImGui();
    void DockspaceAndMenu();

    Entity _directionalLight;

    glm::vec2 _camRotation {0.f};

    Ptr<HierarchyWindow> _hierarchy;
    Ptr<InspectorWindow> _inspector;

    Ptr<GLFrameBuffer> _first;
    Ptr<GLFrameBuffer> _second;

    bool _lock = false;

    std::string _scenePath;
};

