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
     * @brief Struct holding parameters for creating a 3D pointcloud
     */
    struct PointCloudParams
    {
        Vector3 position = {0.0f, 0.0f, 0.0f};         ///< Pointcloud base frame position in world space.
        Quaternion orientation = {0.0, 0.0, 0.0, 1.0}; ///< Pointcloud base frame  orientation as a quaternion.
        Color color = {255, 255, 255, 255};            ///< Color (RGBA format).
        float scale = 1.0;
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;
        PointCloud::MarkerType marker_type = PointCloud::MarkerType::Cube;
        PointCloud::ColoringMode coloring_mode = PointCloud::ColoringMode::SolidColor;
    
    };

    /**
     * @brief Struct for updating model properties dynamically.
     *        Uses std::optional to allow partial updates without allocating heap memory.
     */
    struct PointCloudUpdateParams
    {
        Entity entity;
        std::optional<Vector3> position{};
        std::optional<Quaternion> orientation{};
        std::optional<Color> color{};
        std::optional<bool> visible{};
        std::optional<float> scale{};
        std::optional<PointCloud::MarkerType> marker_type{};
        std::optional<PointCloud::ColoringMode> coloring_mode{};  // New option
    };

} // namespace rbvs
