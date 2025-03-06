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
    class PointCloudRenderingSystem
    {
    public:
        void load_shader(); // Init function
        void update(EntityManager &em,  Camera3D camera);

    private:
        Shader shader;
        Mesh base_mesh;
    };


    // Vector3 cube_vertices[] = {
    //     // Front face
    //     { -0.5f, -0.5f,  0.5f },
    //     {  0.5f, -0.5f,  0.5f },
    //     {  0.5f,  0.5f,  0.5f },
    //     { -0.5f,  0.5f,  0.5f },
    //     // // Back face
    //     { -0.5f, -0.5f, -0.5f },
    //     {  0.5f, -0.5f, -0.5f },
    //     {  0.5f,  0.5f, -0.5f },
    //     { -0.5f,  0.5f, -0.5f }
    // };

    // unsigned short cube_indices[] = {
    //     //0, 1, 2,
    //     // Front face
    //     0, 1, 2, 2, 3, 0,
    //     // Right face
    //     1, 5, 6, 6, 2, 1,
    //     // Back face
    //     5, 4, 7, 7, 6, 5,
    //     // Left face
    //     4, 0, 3, 3, 7, 4,
    //     // Top face
    //     3, 2, 6, 6, 7, 3,
    //     // Bottom face
    //     4, 5, 1, 1, 0, 4
    // };

    // //int cube_index_count = sizeof(cube_indices) / sizeof(cube_indices[0]);


    // Vector3 square_vertices[] = {
    //     // Front face
    //     { -0.5f, -0.5f,  0.5f },
    //     {  0.5f, -0.5f,  0.5f },
    //     {  0.5f,  0.5f,  0.5f },
    //     { -0.5f,  0.5f,  0.5f },
    // };

    // unsigned short square_indices[] = {
    //     // Front face
    //     0, 1, 2, 2, 3, 0,
    // };

    // //int square_index_count = sizeof(square_indices) / sizeof(square_indices[0]);

}
