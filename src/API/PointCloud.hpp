/**
 * @brief Structs for model entities creation and updating
 * @author Eduardo Lopez : eduardo98m@gmail.com
 * 
 */
#pragma once
// Raylib
#include <raylib.h>
// Standart Library
#include <string>
#include <optional>
#include "Components/Base.hpp"
// ECS
#include "ECS/Entity.hpp"

namespace rbvs
{

    /**
     * @brief Struct holding parameters for creating a 3D model.
     */
    struct PointCloudParams
    {
        Vector3 position = {0.0f, 0.0f, 0.0f};         ///< Pointcloud base frame position in world space.
        Quaternion orientation = {0.0, 0.0, 0.0, 1.0}; ///< Pointcloud base frame  orientation as a quaternion.
        Color color = {255, 255, 255, 255};            ///< Color (RGBA format).
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;
    
    };

    /**
     * @brief Struct for updating model properties dynamically.
     *        Uses std::optional to allow partial updates without allocating heap memory.
     */
    struct PointCloudUpdateParams
    {
        Entity entity; // The entity to which we will be updating the params
        std::optional<Vector3> position{};       ///< Optional new position.
        std::optional<Quaternion> orientation{}; ///< Optional new orientation.
        std::optional<Vector3> scale{};          ///< Optional new scale.
        std::optional<Color> color{};          ///< Color (RGBA format).
        std::optional<bool> visible{};
    };

} // namespace rbvs
