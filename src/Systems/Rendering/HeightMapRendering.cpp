#include "Systems/Rendering/HeightMapRendering.hpp"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <memory>

namespace rbvs
{
    void HeightMapRenderingSystem::load_shader()
    {
        std::string vs_path = std::string(SHADER_BASE_PATH) + "/height_map.vs";
        std::string fs_path = std::string(SHADER_BASE_PATH) + "/height_map.fs";

        this->shader = LoadShader(TextFormat(vs_path.c_str(), GLSL_VER), TextFormat(fs_path.c_str(), GLSL_VER));
    }

    void HeightMapRenderingSystem::update(EntityManager &em, Camera3D camera)
    {
        auto view = em.view<HeightMap>();
        Matrix projection = rlGetMatrixProjection();
        Matrix view_mat = GetCameraMatrix(camera);

        for (auto entity : view)
        {
            auto &hm = em.getComponent<HeightMap>(entity);

            if (!hm.visible)
                continue;

            Material mat = LoadMaterialDefault();

            mat.maps[MaterialMapIndex::MATERIAL_MAP_DIFFUSE].color = hm.color;

            Matrix translation_mat = MatrixTranslate(hm.position.x, hm.position.y, hm.position.z);
            Matrix rotation_mat = QuaternionToMatrix(hm.orientation);
            // Use hm.scale.x and hm.scale.y for horizontal dimensions; hm.scale.z as height multiplier.
            Matrix scaling_mat = MatrixScale(hm.scale.x, hm.scale.y, hm.scale.z);
            Matrix model_mat = MatrixMultiply(MatrixMultiply(rotation_mat, scaling_mat), translation_mat);

            Material wires_mat = LoadMaterialDefault();
            wires_mat.maps[MaterialMapIndex::MATERIAL_MAP_DIFFUSE].color = WHITE;

            rlDisableBackfaceCulling();
            // if (hm.color.a < 255)rlDisableDepthMask();
            DrawMesh(*hm.mesh, mat, model_mat);
            // if (hm.color.a < 255)rlEnableDepthMask();
            rlEnableWireMode();
            DrawMesh(*hm.mesh, wires_mat, model_mat);
            rlDisableWireMode();
            // rlEnableBackfaceCulling();

            // // --- Build the model matrix ---

            // int hmWidth = hm.heights.width;
            // int hmHeight = hm.heights.height;

            // // Create a VAO for rendering
            // int heightmapVao = rlLoadVertexArray();
            // rlEnableVertexArray(heightmapVao);

            // // Bind the shader
            // rlEnableShader(shader.id);

            // // Set shader uniforms
            // SetShaderValueMatrix(shader, GetShaderLocation(shader, "modelMatrix"), model_mat);
            // SetShaderValueMatrix(shader, GetShaderLocation(shader, "viewMatrix"), view_mat);
            // SetShaderValueMatrix(shader, GetShaderLocation(shader, "projectionMatrix"), projection);
            // SetShaderValue(shader, GetShaderLocation(shader, "maxDrawDistance"), &this->draw_distance, SHADER_UNIFORM_FLOAT);

            // // Bind the heightmap texture
            // //rlActiveTextureSlot(0);
            // rlEnableTexture(hm.heights.id);
            // SetShaderValueTexture(shader, GetShaderLocation(shader, "heights"), hm.heights);

            // // Draw vertices as a grid of points (each point will be processed in vertex shader)
            // // Using GL_POINTS for vertex-based rendering approach
            // rlSetVertexAttribute(0, 3, RL_FLOAT, false, 0, 0);
            // rlEnableVertexAttribute(0);

            // // Draw heightmap grid as points (will be processed in vertex shader)
            // int numVertices = hmWidth * hmHeight;
            // rlDrawVertexArray(0, numVertices);
            // DrawRectangle(0, 0, 0, 0, WHITE);

            // // Cleanup
            // rlDisableVertexArray();
            // rlDisableTexture();
            // rlDisableShader();
        }
    }
}
