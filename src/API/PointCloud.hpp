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
     * @brief Templated struct holding parameters for creating a 3D pointcloud
     */
    template <typename PointT>
    struct PointCloudParams
    {
        Vector3 position = {0.0f, 0.0f, 0.0f};             ///< Pointcloud base frame position in world space.
        Quaternion orientation = {0.0, 0.0, 0.0, 1.0};     ///< Pointcloud base frame orientation.
        Color color = {255, 255, 255, 255};                ///< Used only if ColoringMode::SolidColor
        float scale = 1.0f;

        typename pcl::PointCloud<PointT>::Ptr cloud = std::make_shared<pcl::PointCloud<PointT>>();

        PointCloudStyle::MarkerType marker_type = PointCloudStyle::MarkerType::Cube;
        PointCloudStyle::ColoringMode coloring_mode = PointCloudStyle::ColoringMode::SolidColor;
    };

    /**
     * @brief Struct for updating pointcloud properties dynamically.
     *        Uses std::optional to allow partial updates without allocations.
     */
    struct PointCloudUpdateParams
    {
        Entity entity;
        std::optional<Vector3> position{};
        std::optional<Quaternion> orientation{};
        std::optional<Color> color{};
        std::optional<bool> visible{};
        std::optional<float> scale{};
        std::optional<PointCloudStyle::MarkerType> marker_type{};
        std::optional<PointCloudStyle::ColoringMode> coloring_mode{};
    };

} // namespace rbvs

