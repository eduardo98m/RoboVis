#include "VisualizerNew.hpp"
namespace rbvs
{
    /**
     * @brief Update common properties of a point cloud entity. This is not templated,
     *        since it only modifies metadata (not the point type itself).
     */
    void Visualizer::update_point_cloud(PointCloudUpdateParams params)
    {
        // You may need to do type-erased lookup here if you store multiple PointCloud<T> specializations.
        // For now, assume you’re updating PointCloud<pcl::PointXYZ>.
        auto &pc = this->entity_manager.getComponent<PointCloud<pcl::PointXYZ>>(params.entity);

        if (params.position) pc.position = *params.position;
        if (params.orientation) pc.orientation = *params.orientation;
        if (params.color) pc.color = *params.color;
        if (params.visible) pc.visible = *params.visible;
        if (params.scale) pc.scale = *params.scale;
        if (params.marker_type) pc.marker_type = *params.marker_type;
        if (params.coloring_mode) pc.coloring_mode = *params.coloring_mode;
    }

    void Visualizer::delete_point_cloud(Entity entity)
    {
        this->entity_manager.destroy(entity);
    }
}
