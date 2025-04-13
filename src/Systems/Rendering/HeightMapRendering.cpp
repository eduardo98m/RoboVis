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

            mat.maps[MaterialMapIndex::MATERIAL_MAP_DIFFUSE].color = WHITE;
            

            Matrix translation_mat = MatrixTranslate(hm.position.x, hm.position.y, hm.position.z);
            Matrix rotation_mat = QuaternionToMatrix(hm.orientation);
            // Use hm.scale.x and hm.scale.y for horizontal dimensions; hm.scale.z as height multiplier.
            Matrix scaling_mat = MatrixScale(hm.scale.x, hm.scale.y, hm.scale.z);
            Matrix model_mat = MatrixMultiply(MatrixMultiply(rotation_mat, scaling_mat), translation_mat);

            Material wires_mat = LoadMaterialDefault();
            wires_mat.maps[MaterialMapIndex::MATERIAL_MAP_DIFFUSE].color = WHITE;

            mat.maps[MaterialMapIndex::MATERIAL_MAP_DIFFUSE].texture = hm.color_map;
            rlDisableBackfaceCulling();
            // if (hm.color.a < 255)rlDisableDepthMask();
            DrawMesh(*hm.mesh, mat, model_mat);
            // if (hm.color.a < 255)rlEnableDepthMask();
            rlEnableWireMode();
            DrawMesh(*hm.mesh, wires_mat, model_mat);
            rlDisableWireMode();
            // rlEnableBackfaceCulling();

        }
    }
}
