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

namespace rbvs {
/**
 * @brief Struct holding parameters for creating a heightmap.
 */
struct HeightMapParams {
  Vector3 position = {0.0f, 0.0f, 0.0f}; ///< Model position in world space.
  Quaternion orientation = {0.0, 0.0, 0.0,
                            1.0};     ///< Model orientation as a quaternion.
  Vector3 scale = {1.0f, 1.0f, 1.0f}; ///< Scaling factors for the model.
  std::vector<float> &heights;
  size_t n_x = 10;
  size_t n_y = 10;
  Color color = WHITE; ///< Color (RGBA format).
  std::optional<std::vector<Color>> color_map = {};
};

/**
 * @brief Struct for updating model properties dynamically.
 *        Uses std::optional to allow partial updates without allocating heap
 * memory.
 */
struct HeightMapUpdateParams {
  Entity entity; // The entity to which we will be updating the params
  std::optional<Vector3> position{};       ///< Optional new position.
  std::optional<Quaternion> orientation{}; ///< Optional new orientation.
  std::optional<Vector3> scale{};          ///< Optional new scale.
  std::optional<std::vector<float>> heights{};
  std::optional<size_t> n_x{};
  std::optional<size_t> n_y{};
  std::optional<Color> color{}; ///< Color (RGBA format).
  std::optional<std::vector<Color>>
      color_map{}; // RGBA Array that will be used to create the texture
  std::optional<bool> visible{};
};

} // namespace rbvs
