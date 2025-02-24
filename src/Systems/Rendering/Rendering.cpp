#include "Systems/Rendering/Rendering.hpp"

namespace rbvs
{

    void RenderingSystem::update(EntityManager& em, Camera3D camera) {
        
        BeginDrawing();
        ClearBackground({30, 30, 30, 255});
        // 3D Rendering
        BeginMode3D(camera);
        visual_model_renderer.update(em);
        EndMode3D();
        rlImGuiBegin();
        //processUI(em);
        entity_setting_system.render_gui(em);
        rlImGuiEnd();
        EndDrawing();
        
    }
        


    
} // namespace rbvs
