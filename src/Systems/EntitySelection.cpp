#include "Systems/EntitySelection.hpp"

namespace rbvs {

// Helper function to create a world transformation matrix
Matrix MakeWorldTransform(Vector3 position, Quaternion orientation,
                          Vector3 scale) {
  Matrix mat_scale = MatrixScale(scale.x, scale.y, scale.z);
  Matrix mat_rot = QuaternionToMatrix(orientation);
  Matrix mat_trans = MatrixTranslate(position.x, position.y, position.z);
  return MatrixMultiply(MatrixMultiply(mat_scale, mat_rot), mat_trans);
}

Matrix MakeWorldTransform(Vector3 position, Quaternion orientation,
                          float uniform_scale) {
  // Matrix mat_scale = MatrixScale(uniform_scale, uniform_scale,
  // uniform_scale);
  Matrix mat_rot = QuaternionToMatrix(orientation);
  Matrix mat_trans = MatrixTranslate(position.x, position.y, position.z);
  return MatrixMultiply(mat_rot, mat_trans);
}

BoundingBox GetTransformedBoundingBox(BoundingBox bbox, Matrix tf) {
  return BoundingBox{
      .min = Vector3Transform(bbox.min, tf),
      .max = Vector3Transform(bbox.max, tf),

  };
}

std::optional<Entity> EntitySelectionSystem::get_selected_entity() const {
  return selected_entity;
}

void EntitySelectionSystem::handle_click(EntityManager &em, Camera3D camera) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    Ray mouse_ray = GetMouseRay(GetMousePosition(), camera);
    float min_distance = std::numeric_limits<float>::max();
    std::optional<Entity> potential_selection = std::nullopt;

    // --- 1. Check VisualModels ---
    auto view_vm = em.view<VisualModel>();
    for (const auto e : view_vm) {
      auto &vm = em.getComponent<VisualModel>(e);

      if (!vm.visible || !vm.model) {
        continue;
      }

      Matrix world_transform =
          MakeWorldTransform(vm.position, vm.orientation, vm.scale);
      BoundingBox local_bbox = GetModelBoundingBox(*vm.model);
      BoundingBox world_bbox =
          GetTransformedBoundingBox(local_bbox, world_transform);
      RayCollision collision = GetRayCollisionBox(mouse_ray, world_bbox);

      // Alternative, potentially more accurate for complex models:
      // RayCollision collision = GetRayCollisionModel(mouse_ray, *vm.model,
      // world_transform);

      if (collision.hit && collision.distance < min_distance) {
        min_distance = collision.distance;
        potential_selection = std::make_optional(e);
      }
    }

    // --- 2. Check PointClouds (pcl::PointXYZRGBA) ---
    auto view_pc_xyz_rgba = em.view<PointCloud<pcl::PointXYZRGBA>>();
    for (const auto e : view_pc_xyz_rgba) {
      auto &pc = em.getComponent<PointCloud<pcl::PointXYZRGBA>>(e);

      if (!pc.visible || !pc.cloud || pc.cloud->points.empty()) {
        continue;
      }

      Matrix world_transform_pc =
          MakeWorldTransform(pc.position, pc.orientation, pc.scale);
      BoundingBox local_pc_bbox = ComputePointCloudBoundingBox(*pc.cloud);
      BoundingBox world_pc_bbox =
          GetTransformedBoundingBox(local_pc_bbox, world_transform_pc);
      RayCollision collision = GetRayCollisionBox(mouse_ray, world_pc_bbox);

      if (collision.hit && collision.distance < min_distance) {
        min_distance = collision.distance;
        potential_selection = std::make_optional(e);
      }
    }

    // --- 3. Check PointClouds (pcl::PointXYZ) ---
    auto view_pc_xyz = em.view<PointCloud<pcl::PointXYZ>>();
    for (const auto e : view_pc_xyz) {
      auto &pc = em.getComponent<PointCloud<pcl::PointXYZ>>(e);

      if (!pc.visible || !pc.cloud || pc.cloud->points.empty()) {
        continue;
      }

      Matrix world_transform_pc =
          MakeWorldTransform(pc.position, pc.orientation, pc.scale);
      BoundingBox local_pc_bbox = ComputePointCloudBoundingBox(*pc.cloud);
      BoundingBox world_pc_bbox =
          GetTransformedBoundingBox(local_pc_bbox, world_transform_pc);
      RayCollision collision = GetRayCollisionBox(mouse_ray, world_pc_bbox);

      if (collision.hit && collision.distance < min_distance) {
        min_distance = collision.distance;
        potential_selection = std::make_optional(e);
      }
    }

    // --- 4. Check HeightMaps ---
    auto view_hm = em.view<HeightMap>();
    for (const auto e : view_hm) {
      auto &hm = em.getComponent<HeightMap>(e);

      if (!hm.visible || !hm.mesh) {
        continue;
      }

      Matrix world_transform_hm =
          MakeWorldTransform(hm.position, hm.orientation, hm.scale);
      RayCollision collision =
          GetRayCollisionMesh(mouse_ray, *hm.mesh, world_transform_hm);

      if (collision.hit && collision.distance < min_distance) {
        min_distance = collision.distance;
        potential_selection = std::make_optional(e);
      }
    }

    // Update the selected entity
    this->selected_entity = potential_selection;

    // Optional: Add feedback if something was selected
    if (this->selected_entity) {
      std::cout << "Selected entity ID: " << selected_entity->id
                << /* some way to get ID from Entity e */ std::endl;
    } else {
      std::cout << "No entity selected." << std::endl;
    }
  }
  // If you want to deselect when clicking on empty space, and nothing was hit
  // this frame: The current logic sets selected_entity to nullopt if nothing is
  // hit. If IsMouseButtonReleased is also important, you might add logic here.
}

} // namespace rbvs
