/**
 * @brief File
 */
#pragma once


// Raylib
#include <raylib.h>
#include <raymath.h>


// Internal
#include "Components/Base.hpp"
#include "ECS/EntityManager.hpp"

#include <optional> // Para selectedEntity


namespace rbvs
{
    class EntitySelectionSystem
    {
    public:
        void handle_click(EntityManager &em,  Camera3D camera);

        /**
         * @brief Gets the entity that is currently selected
         */
        std::optional<Entity> get_selected_entity() const;

    private:
        std::optional<Entity> selected_entity;
    };


    template <typename PointT>
    BoundingBox ComputePointCloudBoundingBox(const pcl::PointCloud<PointT>& cloud)
    {
        if (cloud.points.empty())
        {
            return BoundingBox{Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}};
        }

        Vector3 min_pt = {
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()};
        Vector3 max_pt = {
            -std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max()};

        for (const auto& point : cloud.points)
        {
            min_pt.x = std::min(min_pt.x, point.x);
            min_pt.y = std::min(min_pt.y, point.y);
            min_pt.z = std::min(min_pt.z, point.z);

            max_pt.x = std::max(max_pt.x, point.x);
            max_pt.y = std::max(max_pt.y, point.y);
            max_pt.z = std::max(max_pt.z, point.z);
        }
        return BoundingBox{min_pt, max_pt};
    }
}
