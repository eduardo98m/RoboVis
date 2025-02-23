
#include "Systems/Rendering/VisualModelRendering.hpp"

namespace rbvs{
    void VisualModelRenderingSystem::update(EntityManager& em) {
        auto view = em.view<VisualModel>();
        
        for(auto entity : view) {
            auto& vm = em.getComponent<VisualModel>(entity);
            
            Vector3 axis;
            float angle;
            QuaternionToAxisAngle(vm.orientation, &axis, &angle);
            
            // Actual rendering logic here
            DrawModelEx(*vm.model, 
                        vm.position, 
                        axis, 
                        angle,
                        vm.scale, 
                        vm.color);
        }
    }
}
