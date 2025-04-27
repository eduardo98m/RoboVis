#pragma once
#include "ECS/EntityManager.hpp"
#include "Components/Base.hpp"
#include "Systems/Rendering/VisualModelRendering.hpp"
#include "Systems/Rendering/PointCloudRendering.hpp"
#include "Systems/Rendering/HeightMapRendering.hpp"
#include "Systems/Rendering/ShaderManager.hpp"
#include "Systems/GUI/EntitySettings.hpp"
#include "Systems/Rendering/GizmoRendering.hpp"
#include "Systems/Rendering/GUIRendering.hpp"

#include "rlImGui.h"
#include <raymath.h>
#include <rlgl.h>


namespace rbvs {


    
    /**
     * @brief Handles rendering of all visual components
     */
    class RenderingSystem {
    public:
        void init(void);    

        void update(EntityManager& em, Camera3D camera, const std::map<std::string, std::function<void(void)>> &user_guis = {});
    
    private:
        VisualModelRenderingSystem visual_model_renderer;
        PointCloudRenderingSystem point_cloud_renderer;
        HeightMapRenderingSystem height_map_rendering;
        EntitySettingsSystem entity_setting_system;
        GizmoRenderingSystem gizmo_renderer;

        ShaderManagerSystem shader_manager_system = ShaderManagerSystem();
    };

} // namespace rbvs
