#include "VisualizerNew.hpp"

namespace rbvs
{

  Visualizer::Visualizer(int screen_width, int screen_height, const char *title)
      : gizmo_interaction_system(this->entity_manager)
  {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, title);

    SetTargetFPS(60);
    rlImGuiSetup(true); // Setup ImGui
    this->set_up_camera();
    this->pre_load_meshes();
    this->rendering_system.init();
    ;
  };

  void Visualizer::set_up_camera()
  {
    this->camera = {.position = {0.0f, 10.0f, 10.0f},
                    .target = {0.0f, 0.0f, 0.0f},
                    .up = {0.0f, 1.0f, 0.0f},
                    .fovy = 45.0f,
                    .projection = CAMERA_PERSPECTIVE};
  }

  void Visualizer::update()
  {
    camera_control_system.update_camera(this->camera);
    rendering_system.update(entity_manager, this->camera, this->user_guis);
    entity_selection_system.handle_click(entity_manager, this->camera);
    gizmo_interaction_system.update(
        entity_manager, this->camera,
        entity_selection_system.get_selected_entity());
  }

  void Visualizer::close()
  {
    // // Unload all the models
    // for (auto &vis_object : this->visual_objects_)
    // {
    //     UnloadModel(vis_object->model);
    // }
    rlImGuiShutdown();
    // UnloadRenderTexture(this->shader_target_);
    CloseWindow();
  }

  Visualizer::~Visualizer()
  {
    this->close();

    // Free the memory from the loaded meshes and materiasl
    for (auto &[name, model] : registered_models)
    {
      for (int i = 0; i < model.meshCount; ++i)
      {
        UnloadMesh(model.meshes[i]);
        UnloadMaterial(model.materials[i]);
      }
      MemFree(model.meshes);
      MemFree(model.materials);
    }
  }

} // namespace rbvs
