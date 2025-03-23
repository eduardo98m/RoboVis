/**
 * @author Eduardo I. Lopez H. (eduardo98m@gmail.com)
 * @brief Grid shader
 */
#pragma once
// Raylib
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
// Standar Library
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
