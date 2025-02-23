#pragma once
// Raylib
#include<raylib.h>
#include<raymath.h>
// Internal
#include "ECS/EntityManager.hpp"
#include "Components/Base.hpp"


namespace rbvs{
    class VisualModelRenderingSystem{
        public:
            void update(EntityManager& em);
            
    };

}
