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
        /**
         * @brief Function that initializes the renderer by loading the corresponding shaders
         */
        void load_shader(); // Init function
        /**
         * @brief Update function called each rendering cycle
         * @param em : The entity manager reference.
         * @param camera : The camera object (needed for rendering operations) 
         */
        void update(EntityManager &em,  Camera3D camera);

    private:
        void update_shader_values(Shader &shdr, 
            const Camera &camera, 
            float scale,
            const Matrix &model, 
            const Matrix &view,
            const Matrix &projection,
            const PointCloudStyle::MarkerType &marker_type,
            const PointCloudStyle::ColoringMode &coloring_mode,
            const Color &point_color);


        Shader shader;
        Shader color_pc_shader;
        float draw_distance = 100.0f;


        const Vector3 cube_vertices[8] = {
            // Front face
            { -0.5f, -0.5f,  0.5f },
            {  0.5f, -0.5f,  0.5f },
            {  0.5f,  0.5f,  0.5f },
            { -0.5f,  0.5f,  0.5f },
            // // Back face
            { -0.5f, -0.5f, -0.5f },
            {  0.5f, -0.5f, -0.5f },
            {  0.5f,  0.5f, -0.5f },
            { -0.5f,  0.5f, -0.5f }
        };
        
        const unsigned short cube_indices[36] = {
            //0, 1, 2,
            // Front face
            0, 1, 2, 2, 3, 0,
            // Right face
            1, 5, 6, 6, 2, 1,
            // Back face
            5, 4, 7, 7, 6, 5,
            // Left face
            4, 0, 3, 3, 7, 4,
            // Top face
            3, 2, 6, 6, 7, 3,
            // Bottom face
            4, 5, 1, 1, 0, 4
        };
        
        const Vector3 square_vertices[4] = {
            // Front face
            { -0.5f, -0.5f,  0.5f },
            {  0.5f, -0.5f,  0.5f },
            {  0.5f,  0.5f,  0.5f },
            { -0.5f,  0.5f,  0.5f },
        };
        
        const unsigned short square_indices[6] = {
            // Front face
            0, 1, 2, 2, 3, 0,
        };
    };
}
