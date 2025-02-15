#include "UserPerspectiveCamera.hpp"
#include <API/Time.hpp>
#include <API/Input.hpp>
#include <Rendering/Renderer/RenderApi.hpp>

namespace Rvl
{
    Ref<UserPerspectiveCamera> UserPerspectiveCamera::New(const glm::vec3& position, f32 fov)
    {
        return NewRef<UserPerspectiveCamera>(position, fov);
    }

    UserPerspectiveCamera::UserPerspectiveCamera(const glm::vec3& position, f32 fov)
    {
        _camera = NewRef<PerspectiveCamera>(position, fov);

        Position = Property<glm::vec3>
        (
            &_camera->_position,
            GET { return _camera->GetPosition(); },
            SET { _camera->SetPosition(value); }
        );
    }

    UserPerspectiveCamera::~UserPerspectiveCamera() {}
    
    Ref<Camera> UserPerspectiveCamera::GetCamera() const
    {
        return _camera;
    }

    void UserPerspectiveCamera::Move(Direction direction, f32 speed, TimeStep dt)
    {
        switch (direction)
        {
            case Direction::Forward:
                _camera->_position += _camera->_forward * speed * (f32)dt;
                break;

            case Direction::Back:
                _camera->_position -= _camera->_forward * speed * (f32)dt;
                break;

            case Direction::Left:
                _camera->_position += _camera->_right * speed * (f32)dt;
                break;   

            case Direction::Right:
                _camera->_position -= _camera->_right * speed * (f32)dt;
                break;

            case Direction::ForwardHorizontal:
                _camera->_position += glm::vec3(_camera->_forward.x, 0.f, _camera->_forward.z) * speed * (f32)dt;
                break;

            case Direction::BackHorizontal:
                _camera->_position -= glm::vec3(_camera->_forward.x, 0.f, _camera->_forward.z) * speed * (f32)dt;
                break;
        }
    }

    void UserPerspectiveCamera::Rotate(f32 x, f32 y, f32 z)
    {
        _camera->Rotate(x, y, z);
        _camera->ResetRotation();
    }

    glm::vec3 UserPerspectiveCamera::GetRotationAngles() const
    {
        return _camera->_vecRotation;
    }

    void UserPerspectiveCamera::UpdateControls(ControllerType type, f32 speed, TimeStep dt)
    {
        if (type == ControllerType::Free) 
        {
            if (Input::IsKeyPressed(Keys::Key_W))
                Move(Direction::Forward, speed);

            if (Input::IsKeyPressed(Keys::Key_S))
                Move(Direction::Back, speed);

            if (Input::IsKeyPressed(Keys::Key_A))
                Move(Direction::Left, speed);
                
            if (Input::IsKeyPressed(Keys::Key_D))
                Move(Direction::Right, speed);
        } 
        else if (type == ControllerType::InPlane) 
        {
            if (Input::IsKeyPressed(Keys::Key_W))
                Move(Direction::ForwardHorizontal, 5.f);

            if (Input::IsKeyPressed(Keys::Key_S))
                Move(Direction::BackHorizontal, 5.f);

            if (Input::IsKeyPressed(Keys::Key_A))
                Move(Direction::Left, 5.f);
                
            if (Input::IsKeyPressed(Keys::Key_D))
                Move(Direction::Right, 5.f);

            if (Input::IsKeyPressed(Keys::Key_Space))
                Position->y += 5.f * dt;

            if (Input::IsKeyPressed(Keys::Key_LeftShift))
                Position->y -= 5.f * dt;

        } 
    }

    void UserPerspectiveCamera::UpdateCursorRotation(f32 speed)
    {
        glm::vec2 viewport = RenderApi::GetViewport();

        _rotation += Input::GetCursorDelta() / (f32)viewport.y * speed;
        
        if (_rotation.y > glm::radians(89.f))
            _rotation.y = glm::radians(89.f);

        if (_rotation.y < -glm::radians(89.f))
            _rotation.y = -glm::radians(89.f);

        Rotate(_rotation.y, _rotation.x, 0.f);
    }

}
