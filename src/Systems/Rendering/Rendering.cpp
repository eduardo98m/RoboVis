#include "Systems/Rendering/Rendering.hpp"
#include "Systems/Rendering/GUIRendering.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include <raylib.h>

namespace rbvs {

void RenderingSystem::init(void) {
  shader_manager_system.load_grid_shader();
  point_cloud_renderer.load_shader();
  height_map_rendering.load_shader();
  lighting_system.init();
  shadow_map_system.init();
}

void RenderingSystem::update(
    EntityManager &em, Camera3D camera,
    const std::map<std::string, std::function<void(void)>> &user_guis) {

  BeginDrawing();
  ClearBackground({30, 30, 30, 255});
  shadow_map_system.update(em, camera, lighting_system.lit_shader());
  BeginMode3D(camera);

  shader_manager_system.render_grid_shader(camera);
  lighting_system.update_light_values(em, camera);
  visual_model_renderer.update(em, camera);
  point_cloud_renderer.update(em, camera);
  height_map_rendering.update(em, camera);
  map_renderer.update(em, camera);
  gizmo_renderer.update(em);

  rlActiveTextureSlot(1);
  rlDisableTexture();

  EndMode3D();
  rlImGuiBegin();
  entity_setting_system.render_gui(em);

  // User defined imgui interfaces
  render_gui(user_guis);

  // Setings for the gizmos
  gizmo_renderer.render_settings_gui();

  // Check for the rendering system
  ImGui::Begin("Shadow Map Texture Vis");
  rlImGuiImage(&this->shadow_map_system.shadow_map.depth);
  ImGui::End();

  rlImGuiEnd();
  EndDrawing();
}

} // namespace rbvs
