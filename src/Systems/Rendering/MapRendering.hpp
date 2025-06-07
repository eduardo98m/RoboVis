#pragma once
// Raylib
#include<raylib.h>
#include<raymath.h>
#include<rlgl.h>
// STD
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

// Internal
#include "ECS/EntityManager.hpp"
#include "Components/Base.hpp"


namespace rbvs{
    class MapRenderingSystem{
        private:
            // Setings 
            float rendering_distance = 120; // [m]

        public:
            /**
             * @brief 
             */
            void update(EntityManager &em, const Camera3D &camera);

            /**
             * @brief 
             */
            void update_chunks(Map &map, const Vector3 &camera_pos);


            void start_loader_thread(Map &map);
            
    };

}
