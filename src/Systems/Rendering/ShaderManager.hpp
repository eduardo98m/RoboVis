#pragma once
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <string>

#define GLSL_VER 440

namespace rbvs
{

    class ShaderManagerSystem
    {
        public:

        //ShaderManagerSystem();
        void render_grid_shader(Camera3D camera);
        void load_grid_shader(void);

    private:
        Shader grid_shader;
        Model plane_model;
    };

}
