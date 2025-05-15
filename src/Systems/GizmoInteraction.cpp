#include "Systems/GizmoInteraction.hpp"

namespace rbvs
{
    // --- Implementación de GizmoInteractionSystem ---
    GizmoInteractionSystem::GizmoInteractionSystem(EntityManager &em)
    {   
        // Create the gizmo entity in the entity manager
        this->create_gizmo(em);
    }

    void GizmoInteractionSystem::create_gizmo(EntityManager &em)
    {

        Gizmo gz = {
            .transform = Transform{
                .translation = Vector3{0. , 0., 0.},
                .rotation = Quaternion{0.0 , 0.0 , 0.0, 1.0},
                .scale = Vector3{1., 1., 1.},
            },
            .visible = false   
        };

        gizmo_entity = em.create();
        em.addComponent<Gizmo>(gizmo_entity, std::move(gz));
    }

    void GizmoInteractionSystem::toggleGizmoControls(EntityManager &em, bool active)
    {
        
    }

    void GizmoInteractionSystem::update(EntityManager &em, Camera3D camera, std::optional<Entity> selected_entity)
    {   
        // If the object is selected enable the gizmo
        auto &gizmo = em.getComponent<Gizmo>(this->gizmo_entity);

        if (selected_entity){
            
            gizmo.visible = true;
            bool continued_interaction = *this->previously_selected_entity == *selected_entity;
            
            if (!continued_interaction){
                if(em.hasComponent<PointCloud<pcl::PointXYZRGBA>>(*selected_entity)){
                    auto &pc = em.getComponent<PointCloud<pcl::PointXYZRGBA>>(*selected_entity);
                    setGizmoTransformFromObject(pc, gizmo.transform);
                };
    
                if(em.hasComponent<VisualModel>(*selected_entity)){
                    auto &vm = em.getComponent<VisualModel>(*selected_entity);
                    setGizmoTransformFromObject(vm, gizmo.transform);
                };
                controlsActive = true;
            }
            else {
                if(em.hasComponent<PointCloud<pcl::PointXYZRGBA>>(*selected_entity)){
                    auto &pc = em.getComponent<PointCloud<pcl::PointXYZRGBA>>(*selected_entity);
                    setObjectTransformFromGizmo(pc, gizmo.transform);
                };
    
                if(em.hasComponent<VisualModel>(*selected_entity)){
                    auto &vm = em.getComponent<VisualModel>(*selected_entity);
                    setObjectTransformFromGizmo(vm, gizmo.transform);
                };
                controlsActive = false; 
            }            
        }
        else{
            gizmo.visible = false;
            controlsActive = false; 
        }

        this->previously_selected_entity = selected_entity;
    }


    void GizmoInteractionSystem::render_selection_gui(EntityManager &em)
    {

    }

} // namespace rbvs