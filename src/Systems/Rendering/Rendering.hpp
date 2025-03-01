#pragma once
#include "ECS/EntityManager.hpp"
#include "Components/Base.hpp"
#include "Systems/Rendering/VisualModelRendering.hpp"
#include "Systems/Rendering/PointCloudRendering.hpp"
#include "Systems/Rendering/ShaderManager.hpp"
#include "Systems/GUI/EntitySettings.hpp"

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

        void update(EntityManager& em, Camera3D camera);
    
    private:
        VisualModelRenderingSystem visual_model_renderer;
        PointCloudRenderingSystem point_cloud_renderer;
        EntitySettingsSystem entity_setting_system;

        ShaderManagerSystem shader_manager_system = ShaderManagerSystem();
    };

} // namespace rbvs
