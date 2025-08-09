/**
 * @brief Structs for model entities creation and updating
 * @author Eduardo Lopez : eduardo98m@gmail.com
 *
 */
#pragma once
// Raylib
#include <raylib.h>
// Standart Library
#include "Components/Base.hpp"
#include <optional>
#include <string>
// ECS
#include "ECS/Entity.hpp"

namespace rbvs
{

  /**
   * @brief Enum representing different types of models that can be created.
   */
  enum class ModelType
  {
    SPHERE,   ///< Sphere model.
    BOX,      ///< Axis-aligned bounding box.
    MESH,     ///< Custom mesh model loaded from a file.
    CYLINDER, ///< Cylinder model.
    CONE,     ///< Cone model.
    CUSTOM
  };

  /**
   * @brief
   */
  struct PreLoadedMeshes
  {
    Mesh box_mesh;      // GenMeshCube(1.0,  1.0, 1.0);
    Mesh sphere_mesh;   // GenMeshSphere(1.0,  32, 32);
    Mesh cylinder_mesh; // GenMeshCylinder(1.0,  1.0, 32);
    Mesh cone_mesh;     // GenMeshCone(1.0,  1.0, 32);
  };

  /**
   * @brief Struct holding parameters for creating a 3D model.
   */
  struct ModelParams
  {
    Vector3 position = {0.0f, 0.0f, 0.0f}; ///< Model position in world space.
    Quaternion orientation = {0.0, 0.0, 0.0, 1.0};     ///< Model orientation as a quaternion.
    Vector3 scale = {1.0f, 1.0f, 1.0f}; ///< Scaling factors for the model.
    Color color = {255, 255, 255, 255}; ///< Color (RGBA format).

    ModelType model_type = ModelType::SPHERE;

    // Shape-specific parameters
    float radius = 1.0f;                       ///< Sphere radius or cylinder/cone base radius.
    float length = 1.0f;                       ///< Cylinder length or cone height.
    Vector3 half_extents = {1.0f, 1.0f, 1.0f}; ///< Half-widths for box models.

    std::string model_path =
        ""; ///< File path for custom mesh models (used for ModelType::MESH).

    std::string custom_model_key = "";
  };

  /**
   * @brief Struct for updating model properties dynamically.
   *        Uses std::optional to allow partial updates without allocating heap
   * memory.
   */
  struct ModelUpdateParams
  {
    Entity entity;                           // The entity to which we will be updating the params
    std::optional<Vector3> position{};       ///< Optional new position.
    std::optional<Quaternion> orientation{}; ///< Optional new orientation.
    std::optional<Vector3> scale{};          ///< Optional new scale.
    std::optional<Color> color{};            ///< Color (RGBA format).
    std::optional<bool> visible{};
  };

  /**
   * @brief enum for declaring the model primitive types
   */
  enum ModelPrimitiveType
  {
    SPHERE,
    BOX,
    CYLINDER,
    CONE,
    HEMISPHERE,
  };

  /**
   * @brief Struct for declaring the model primitives
   * */
  struct ModelPrimitive
  {
    Vector3 position = {0.0, 0.0, 0.0};
    Quaternion orientation = {0.0, 0.0, 0.0, 1.0};
    ModelPrimitiveType type = SPHERE;
    float radius = 0.0;
    float height = 0.0;
    Vector3 half_extents = {0.0, 0.0, 0.0};
    Color color = WHITE; // Default color
    // Is it possible to assign other color to other parameters?
  };

} // namespace rbvs
