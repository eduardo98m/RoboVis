#include "Systems/Rendering/Rendering.hpp"

namespace rbvs
{

    void RenderingSystem::init(void)
    {
        shader_manager_system.load_grid_shader();
        point_cloud_renderer.load_shader();
    }

    void RenderingSystem::update(EntityManager &em, Camera3D camera)
    {

        BeginDrawing();
        ClearBackground({30, 30, 30, 255});
        BeginMode3D(camera);

        shader_manager_system.render_grid_shader(camera);
        visual_model_renderer.update(em);
        point_cloud_renderer.update(em, camera);
        gizmo_renderer.update(em);

        EndMode3D();
        rlImGuiBegin();
        // processUI(em);
        entity_setting_system.render_gui(em);

        // Setings for the gizmos
        gizmo_renderer.render_settings_gui();

        rlImGuiEnd();
        EndDrawing();
    }

} // namespace rbvs
