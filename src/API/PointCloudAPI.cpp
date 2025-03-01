#include "VisualizerNew.hpp"

namespace rbvs
{
    Entity Visualizer::create_point_cloud(PointCloudParams params)
    {
        PointCloud pc = {
            .cloud = params.cloud,
            .position = params.position,
            .orientation = params.orientation,
            .color = params.color,
            .visible = true
        };

        Entity e = this->entity_manager.create();
        this->entity_manager.addComponent<PointCloud>(e, std::move(pc));

        return e;
    }

    void Visualizer::update_point_cloud(PointCloudUpdateParams params)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doesn't have the component)
        PointCloud &pc = this->entity_manager.getComponent<PointCloud>(params.entity);
        
        if (params.position) pc.position = *params.position;
        if (params.orientation) pc.orientation = *params.orientation;
        if (params.color) pc.color = *params.color; 
        if (params.visible) pc.visible = *params.visible;      
    }

    void Visualizer::delete_point_cloud(Entity entity)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doesn't have the component)
        this->entity_manager.destroy(entity);   
    }
}
