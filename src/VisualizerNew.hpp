#pragma once
// External
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
// Standar Library
#include <functional>
#include <map>
// Internal
#include "ECS/Entity.hpp"
#include "ECS/EntityManager.hpp"
// Internal - Systems
#include "Systems/EntitySelection.hpp"
#include "Systems/GizmoInteraction.hpp"
#include "Systems/Rendering/Rendering.hpp"
#include "Systems/Serialization.hpp"
#include "Systems/UserCameraInput.hpp"

// Internal - API
#include "API/Gizmo.hpp"
#include "API/HeightMap.hpp"
#include "API/Map.hpp"
#include "API/Model.hpp"
#include "API/PointCloud.hpp"

namespace rbvs {
class Visualizer {
private:
  EntityManager entity_manager;
  RenderingSystem rendering_system;
  EntitySelectionSystem entity_selection_system;
  PreLoadedMeshes pre_loaded_meshes;
  UserCameraInputSystem camera_control_system;
  SerializationSystem serialization_system;
  GizmoInteractionSystem gizmo_interaction_system;

  Camera3D camera;

  std::map<std::string, std::function<void(void)>>
      user_guis; // User defined guis
  std::unordered_map<std::string, Model>
      registered_models; // User defined ModelPrimitives
  void set_up_camera(void);
  void pre_load_meshes(void);

public:
  Visualizer(int screen_width, int screen_height, const char *title);

  ~Visualizer();

  void update();

  void save() {
    this->serialization_system.serialize(
        this->entity_manager, "/home/eduardo/Documents/Moonshoot/ms-viz/",
        "world");
  };

  /**
   * @brief Registers a GUI rendering function to be called each frame.
   *
   * Each GUI function must be associated with a unique name. If the name
   * already exists in the `user_guis` map, the function will not be added.
   *
   * @param name A unique identifier for the GUI function.
   * @param gui_function The GUI function to be invoked during the rendering
   * loop.
   * @return True if the function was successfully added; false if the name
   * already exists.
   */
  bool add_gui(std::string name, std::function<void(void)> gui_function);

  /**
   * @brief Unregisters a GUI rendering function previously added with
   * `add_gui`.
   *
   * The function identified by the given name will be removed from the
   * `user_guis` map.
   *
   * @param name The name of the GUI function to remove.
   * @return True if the function was found and removed; false otherwise.
   */
  bool remove_gui(std::string name);

  /**
   * @brief Creates a model entity
   */
  Entity create_model(ModelParams params);
  /**
   * @brief Updates a model entity
   */
  void update_model(ModelUpdateParams params);

  void delete_model(Entity entity);
  /**
   * @brief Special function that allows the user to create a model (upload it
   * to the GPU), and use it later (as many times at he wants)
   */
  void register_model(std::string model_name,
                      std::initializer_list<ModelPrimitive> primitives);

  template <typename PointT>
  Entity create_point_cloud(PointCloudParams<PointT> params);

  void update_point_cloud(PointCloudUpdateParams params);

  void delete_point_cloud(Entity entity);

  Entity create_gizmo(GizmoParams params);

  void update_gizmo(GizmoUpdateParams params);

  Entity create_height_map(const HeightMapParams &params);

  void update_height_map(const HeightMapUpdateParams &params);

  Entity create_map(MapParams params);

  void update_map(MapUpdateParams params);

  void delete_map(Entity entity);

  // void delete_entity(Entity entity);

  /**
   * @brief
   * */
  void close();
};
}; // namespace rbvs

// Template for hte pointclouds
#include "API/PointCloudAPI.tpp"
