#pragma once
// Raylib
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
// Internal
#include "ECS/EntityManager.hpp"
#include "Systems/Rendering/Lightning.hpp"

namespace rbvs
{
  class VisualModelRenderingSystem
  {
  public:
    void update(EntityManager &em, Camera3D camera);
  };

} // namespace rbvs
