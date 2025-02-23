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
        //processUI(em);
        EndDrawing();
        
    }
        


    
} // namespace rbvs
