#include "VisualizerNew.hpp"

namespace rbvs
{
    Entity Visualizer::create_gizmo(GizmoParams params)
    {

        Gizmo gz = {
            .transform = Transform{
                .translation = params.position,
                .rotation = params.orientation,
                .scale = params.scale,
            }
            
        };


        Entity e = this->entity_manager.create();
        this->entity_manager.addComponent<Gizmo>(e, std::move(gz));

        return e;
    }


    void Visualizer::update_gizmo(GizmoUpdateParams params)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doenst have the component)
        Gizmo &gz = this->entity_manager.getComponent<Gizmo>(params.entity);

        if (params.position) gz.transform.translation = *params.position;
        if (params.orientation) gz.transform.rotation = *params.orientation;
        if (params.scale) gz.transform.scale = *params.scale;
        if (params.visible) gz.visible = *params.visible;      
    }
}
