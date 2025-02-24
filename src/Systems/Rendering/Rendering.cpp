#include "Systems/Rendering/Rendering.hpp"

namespace rbvs
{

    void RenderingSystem::init(void){
        shader_manager_system.load_grid_shader();
    }

    void RenderingSystem::update(EntityManager& em, Camera3D camera) {
        
        BeginDrawing();
        ClearBackground({30, 30, 30, 255});
        BeginMode3D(camera);

       // First render the grid with depth writing disabled
        //rlEnableDepthTest();
        //rlSetBlendMode(RL_BLEND_ALPHA);
        rlDisableDepthMask();  // Disable depth writing for grid
        rlDisableBackfaceCulling();
        shader_manager_system.render_grid_shader(camera);
        rlEnableDepthMask();   // Re-enable depth writing
        
        // Then render opaque objects - they will draw over the grid where needed
        rlEnableBackfaceCulling();
        //rlSetBlendMode(RL_BLEND_ALPHA);
        visual_model_renderer.update(em);
            
        EndMode3D();
        rlImGuiBegin();
        //processUI(em);
        entity_setting_system.render_gui(em);
        rlImGuiEnd();
        EndDrawing();
        
    }
        


    
} // namespace rbvs
