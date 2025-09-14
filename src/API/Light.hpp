/**
 * @brief Structs for light creation and update
 */

#pragma once

#include "Components/Base.hpp"
#include "ECS/Entity.hpp"
#include <optional>
#include <raylib.h>

namespace rbvs {
struct LightParams {
  Vector3 position = {0.0, 0.0, 0.0};
  Vector3 target = {0.0, 0.0, 0.0};
  LightType type = LightType::DIRECTIONAL;
  Color color = WHITE;
  float intensity = 1.0;
  float cut_off = 0.5;
  float outer_cut_off = 20.0;
  bool enabled = true;
};

struct LightUpdateParams {
  Entity entity; // Entity with the light to update
  std::optional<Vector3> position{};
  std::optional<Vector3> target{};
  std::optional<LightType> type{};
  std::optional<Color> color{};
  std::optional<float> intensity{};
  std::optional<float> cut_off{};
  std::optional<float> outer_cut_off{};
  std::optional<bool> enabled{};
};
} // namespace rbvs
