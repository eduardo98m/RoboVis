
#include "Systems/UserCameraInput.hpp"
namespace rbvs
{

    void UserCameraInputSystem::update_camera(Camera3D &camera, float speed, float sensitivity)
    {
        Vector3 cameraFront = Vector3Subtract(camera.target, camera.position);
        Vector3 cameraRight = Vector3Normalize(Vector3CrossProduct(cameraFront, camera.up));

        Vector3 movement = {0.0f, 0.0f, 0.0f};
        Vector3 rotation = {0.0f, 0.0f, 0.0f};

        
        if (IsKeyDown(KEY_W))
            movement.x += speed;
        if (IsKeyDown(KEY_S))
            movement.x -= speed;
        if (IsKeyDown(KEY_A))
            movement.y -= speed;
        if (IsKeyDown(KEY_D))
            movement.y += speed;

        // Vertical movement with Shift (up) and Control (down) keys
        if (IsKeyDown(KEY_LEFT_SHIFT))
            movement.z += speed;
        if (IsKeyDown(KEY_LEFT_CONTROL))
            movement.z -= speed;
        if (IsKeyDown(KEY_Q))
            movement.z += speed;
        if (IsKeyDown(KEY_E))
            movement.z -= speed;

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            rotation.y += (GetMouseDelta().y * sensitivity);
            rotation.x += (GetMouseDelta().x * sensitivity);

            if (rotation.x > 89.0f)
                rotation.x = 89.0f;
            if (rotation.x < -89.0f)
                rotation.x = -89.0f;
        }

        UpdateCameraPro(&camera, movement, rotation, 0.0f);
    }
}; // namespace rbvs