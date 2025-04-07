#pragma once
#include "ECS/EntityManager.hpp"
#include "Components/Base.hpp"
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <vector>
#define GLSL_VER 440

namespace rbvs
{
    class HeightMapRenderingSystem
    {
    public:
        void load_shader(); // Init function
        void update(EntityManager &em,  Camera3D camera);

    private:
        Shader shader;
        float draw_distance = 100.0f;
    };
}
