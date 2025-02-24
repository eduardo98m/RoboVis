
#include "Systems/Rendering/ShaderManager.hpp"
#include <iostream>
#include <rlgl.h>
#include <string.h>
namespace rbvs
{

    void ShaderManagerSystem::load_grid_shader(void)
    {
        std::string vs_path = std::string(SHADER_BASE_PATH) + "/grid.vs";
        std::string fs_path = std::string(SHADER_BASE_PATH) + "/grid.fs";
        std::cout << "Shader loaded\n";

        this->grid_shader = LoadShader(TextFormat(vs_path.c_str(), GLSL_VER), TextFormat(fs_path.c_str(), GLSL_VER));

        // Mesh plane_mesh = GenMeshPlane(2.0f, 2.0f, 1, 1); // 2x2 size, 1x1 segments
        // plane_model = LoadModelFromMesh(plane_mesh);
        // plane_model.materials[0].shader = grid_shader;
    }

    void ShaderManagerSystem::render_grid_shader(Camera3D camera)
    {
        // Compute view and projection matrices
        Matrix view = GetCameraMatrix(camera);
        Matrix projection = MatrixPerspective(
            camera.fovy * DEG2RAD,
            (float)GetScreenWidth() / (float)GetScreenHeight(),
            0.1f,
            1000.0f);

        // Combine matrices (projection * view)
        Matrix viewProj = MatrixMultiply(view, projection);

        
        // SetShaderValueMatrix(grid_shader, GetShaderLocation(grid_shader, "view"), view);
        // SetShaderValueMatrix(grid_shader, GetShaderLocation(grid_shader, "projection"), projection);

        // Update shader uniforms
        int vpLoc = GetShaderLocation(grid_shader, "gVP");
        SetShaderValueMatrix(grid_shader, vpLoc, viewProj);

        // Update gCameraWorldPos with the camera's position.
        int camPosLoc = GetShaderLocation(this->grid_shader, "gCameraWorldPos");
        SetShaderValueV(this->grid_shader, camPosLoc, &camera.position, SHADER_UNIFORM_VEC3, 1);

        
        // rlDisableDepthMask(); // Ensure depth writing is enabled
        //rlSetBlendMode(RL_BLEND_ALPHA);
        BeginShaderMode(this->grid_shader);
        DrawPlane({0.0, 0.0, 0.0}, {100, 100}, WHITE);
        EndShaderMode();
        //rlSetBlendMode(RL_BLEND_ALPHA);


        

        // // Restore state if necessary
        // rlEnableDepthMask();
        // rlDisableDepthTest();
    }
}