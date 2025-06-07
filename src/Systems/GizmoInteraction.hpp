/**
 * @brief
 */
#pragma once

#include "Components/Base.hpp"
#include "ECS/EntityManager.hpp"
#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <optional> // Para selectedEntity

namespace rbvs
{
    class GizmoInteractionSystem
    {
    public:
        GizmoInteractionSystem(EntityManager &em);

        void update(EntityManager &em, Camera3D camera , std::optional<Entity> selected_entity);

        void toggleGizmoControls(EntityManager &em, bool active); 

        void render_selection_gui(EntityManager &em);

    private:
        Entity gizmo_entity;    
        
        std::optional<Entity> previously_selected_entity; // La entidad que está seleccionada actualmente
        bool controlsActive = true;              // Si el sistema de gizmo está activo

        void create_gizmo(EntityManager &em);

        template <typename T>
        void setObjectTransformFromGizmo(T &component, const Transform &gizmoTransform)
        {
            component.position = gizmoTransform.translation;
            component.orientation = gizmoTransform.rotation;
            // component.scale = gizmoTransform.scale;
        };

        template <typename T>
        void setGizmoTransformFromObject(const T &component, Transform &gizmoTransform)
        {
            gizmoTransform.translation = component.position;
            gizmoTransform.rotation = component.orientation;
            // gizmoTransform.scale = component.scale;
        };
    };
}