#pragma once
#include "ECS/EntityManager.hpp"
#include "Components/Base.hpp"
#include "Systems/Rendering/VisualModelRendering.hpp"
#include "Systems/GUI/EntitySettings.hpp"
#include "rlImGui.h"



namespace rbvs {


    
    /**
     * @brief Handles rendering of all visual components
     */
    class RenderingSystem {
    public:
        void update(EntityManager& em, Camera3D camera);
    
    private:
        VisualModelRenderingSystem visual_model_renderer;
        EntitySettingsSystem entity_setting_system;
    };

} // namespace rbvs
