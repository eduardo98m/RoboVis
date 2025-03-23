#pragma once
// External
#include <raylib.h>
#include <raymath.h>
// Standar
#include <vector>
#include <memory>
// Internal

namespace rbvs {

/**
 * @class Camera system for the user
 * @brief System responsible for controlling the user camera
 *
 */
class UserCameraInputSystem {
public:
    /**
     * @brief Updates the camera based on the user input
     *
     * 
     *
     * @param camera Camera3D reference.
     * @param speed Speed of the camera
     * @param sensitivity Sensitivity of the camers
     */
    void update_camera(Camera3D& camera, float speed  = 0.3, float sensitivity = 0.2);
    };
};// namespace rbvs
