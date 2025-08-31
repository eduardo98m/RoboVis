
#include "Systems/Rendering/VisualModelRendering.hpp"
#include "Components/Base.hpp"

namespace rbvs
{
  void VisualModelRenderingSystem::update(EntityManager &em,
                                          Camera3D camera)
  {
    auto view = em.view<VisualModel>();

    for (auto entity : view)
    {
      auto &vm = em.getComponent<VisualModel>(entity);
      if (!vm.visible)
        continue;

      Vector3 axis;
      float angle;
      QuaternionToAxisAngle(vm.orientation, &axis, &angle);

      if (vm.color.a < 255)
        rlDisableDepthMask();

      DrawModelEx(*vm.model, vm.position, axis, angle, vm.scale, vm.color);

      rlEnableDepthMask();
    }
  }

} // namespace rbvs
