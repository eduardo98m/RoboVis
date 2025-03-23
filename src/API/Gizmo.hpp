/**
 * @brief Structs for model entities creation and updating
 * @author Eduardo Lopez : eduardo98m@gmail.com
 * 
 */
#pragma once
// Raylib
#include <raylib.h>
// Standart Library
#include <optional>
#include "Components/Base.hpp"
// ECS
#include "ECS/Entity.hpp"

namespace rbvs
{
    
    /**
     * @brief Struct holding parameters for creating a 3D model.
     */
    struct GizmoParams
    {
        Vector3 position = {0.0f, 0.0f, 0.0f};         ///< Model position in world space.
        Quaternion orientation = {0.0, 0.0, 0.0, 1.0}; ///< Model orientation as a quaternion.
        Vector3 scale = {1.0f, 1.0f, 1.0f};            ///< Scaling factors for the gizmo.
    };

    /**
     * @brief Struct for updating gizmo properties dynamically.
     *        Uses std::optional to allow partial updates without allocating heap memory.
     */
    struct GizmoUpdateParams
    {
        Entity entity; // The entity to which we will be updating the params
        std::optional<Vector3> position{};       ///< Optional new position.
        std::optional<Quaternion> orientation{}; ///< Optional new orientation.
        std::optional<Vector3> scale{};          ///< Optional new scale.
        std::optional<bool> visible{};
    };

} // namespace rbvs
