#pragma once
#include "VisualizerNew.hpp"
namespace rbvs
{
    /**
     * @brief Create a point cloud entity and register it in the entity manager.
     * @tparam PointT The point type (e.g., pcl::PointXYZ, pcl::PointXYZRGB)
     * @param params The point cloud creation parameters.
     * @return The created Entity handle.
     */
    template <typename PointT>
    Entity Visualizer::create_point_cloud(PointCloudParams<PointT> params)
    {
        PointCloud<PointT> pc = {
            .cloud = params.cloud,
            .position = params.position,
            .orientation = params.orientation,
            .color = params.color,
            .marker_type = params.marker_type,
            .coloring_mode = params.coloring_mode,
            .visible = true,
            .scale = params.scale,
        };

        // If coloring mode requires per-point colors, prepare color buffer
        if constexpr (std::is_same_v<PointT, pcl::PointXYZRGBA> || \
                      std::is_same_v<PointT, pcl::PointXYZRGB>)
        {
            pc.pos_buffer.reserve(pc.cloud->points.size());
            pc.color_buffer.reserve(pc.cloud->points.size());

            for (size_t i = 0; i < pc.cloud->size(); ++i)
            {
                const auto &pt = pc.cloud->points[i];
                pc.pos_buffer[i] = {pt.x, pt.y, pt.z, 0.0f};
                pc.color_buffer[i] = {
                    pt.r / 255.0f,
                    pt.g / 255.0f,
                    pt.b / 255.0f,
                    (std::is_same_v<PointT, pcl::PointXYZRGBA> ? pt.a / 255.0f : 1.0f)
                };
            }
        }

        Entity e = this->entity_manager.create();
        this->entity_manager.addComponent<PointCloud<PointT>>(e, std::move(pc));

        return e;
    }
}
