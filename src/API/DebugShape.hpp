/**
 * @brief Structs for debug shapes entities creation and updating
 * @author Eduardo Lopez : eduardo98m@gmail.com
 * 
 */
#pragma once
// Raylib
#include <raylib.h>
// Standart Library
#include <string>
#include <optional>

namespace rbvs
{

    /**
     * @brief Enum representing different types of models that can be created.
     */
    enum class DebugShapeType
    {
        SPHERE,
        ARROW,
        LINE,


    };

    /**
     * @brief Struct holding parameters for creating a 3D model.
     */
    struct DebugShapeParams
    {
        Vector3 position = {0.0f, 0.0f, 0.0f};         ///< Model position in world space.
        Quaternion orientation = {0.0, 0.0, 0.0, 1.0}; ///< Model orientation as a quaternion.
        Color color = {255, 255, 255, 255};            ///< Color (RGBA format).

        // Debug - shape-specific parameters
        float radius = 1.0f;                       ///< Sphere radius or cylinder/cone base radius.
        float length = 1.0f;                       ///< Cylinder length or cone height.
        Vector3 half_extents = {1.0f, 1.0f, 1.0f}; ///< Half-widths for box models.

        std::string mesh_path = ""; ///< File path for custom mesh models (used for ModelType::MESH).
    };

} // namespace rbvs
