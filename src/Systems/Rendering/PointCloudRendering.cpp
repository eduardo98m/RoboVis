#include "Systems/Rendering/PointCloudRendering.hpp"

namespace rbvs
{
    void PointCloudRenderingSystem::load_shader()
    {
        std::string vs_path = std::string(SHADER_BASE_PATH) + "/pointcloud.vs";
        std::string fs_path = std::string(SHADER_BASE_PATH) + "/pointcloud.fs";

        this->shader = LoadShader(TextFormat(vs_path.c_str(), GLSL_VER), TextFormat(fs_path.c_str(), GLSL_VER));
    }

    void PointCloudRenderingSystem::update(EntityManager &em, Camera3D camera)
    {
        auto view = em.view<PointCloud>();

        for (auto entity : view)
        {
            // Get the pointcloud entuty from the entity manager
            auto &pc = em.getComponent<PointCloud>(entity);

            // Check if it is visible and if it has points
            if (!pc.visible || pc.cloud->points.empty())
                continue;

            // --- Compute the matrices:

            //
            Matrix translation = MatrixTranslate(pc.position.x, pc.position.y, pc.position.z);
            Matrix rotation = QuaternionToMatrix(pc.orientation);
            // Model matrix : I.e. Homogeneous transform of the coordinates of the pointcloud
            Matrix model = MatrixMultiply(rotation, translation);
            
            // Calculate the camera matrices
            Matrix projection = rlGetMatrixProjection();
            Matrix view = GetCameraMatrix(camera);

            // Also pass thesese vectors to the shader 
            // (is better to calculate them once in cpu rather that doing it many times on gpu)
            Vector3 view_right = Vector3{view.m0, view.m4, view.m8}; 
            Vector3 view_up = Vector3{view.m1, view.m5, view.m9}; 

            // --- Prepare the VAO [Vertex array object] for instanced rendering ---
            int particleVao = rlLoadVertexArray();
            rlEnableVertexArray(particleVao);
            if (pc.ssboID == 0)
            {   
                // Here we set the Shader Storage Buffer Object ID (is an integer ID that represents the SSBO within OpenGL.)
                pc.ssboID = rlLoadShaderBuffer(pc.cloud->points.size() * sizeof(pcl::PointXYZ), pc.cloud->points.data(), RL_DYNAMIC_COPY);
            }
            else
            {
                rlUpdateShaderBuffer(pc.ssboID, pc.cloud->points.data(), pc.cloud->points.size() * sizeof(pcl::PointXYZ), 0);
            }

            // Get the correct shape type markers
            int index_count;
            if (pc.marker_type == PointCloud::MarkerType::Square){
                // Setup VBO for the cube vertices
                index_count = sizeof(square_indices) / sizeof(square_indices[0]);
                int vbo = rlLoadVertexBuffer(square_vertices, sizeof(square_vertices), false);
                int ibo = rlLoadVertexBufferElement(square_indices, sizeof(square_indices), false);           
            }else{
                // Setup VBO for the cube vertices
                index_count = sizeof(cube_indices) / sizeof(cube_indices[0]);
                int vbo = rlLoadVertexBuffer(cube_vertices, sizeof(cube_vertices), false);
                int ibo = rlLoadVertexBufferElement(cube_indices, sizeof(cube_indices), false);
                
            }
             
            // Configure vertex attributes
            rlEnableVertexAttribute(0);
            rlSetVertexAttribute(0, 3, RL_FLOAT, false, 0, 0);
            rlDisableBackfaceCulling();

            // Start Drawing ---
            rlEnableShader(shader.id);

            // Set shader uniforms
            SetShaderValueMatrix(shader, GetShaderLocation(shader, "projectionMatrix"), projection);
            SetShaderValueMatrix(shader, GetShaderLocation(shader, "viewMatrix"), view);
            SetShaderValueMatrix(shader, GetShaderLocation(shader, "modelMatrix"), model);
            SetShaderValue(shader, GetShaderLocation(shader, "CameraRight_worldspace"), &view_right, SHADER_UNIFORM_VEC3);
            SetShaderValue(shader, GetShaderLocation(shader, "CameraUp_worldspace"), &view_up, SHADER_UNIFORM_VEC3);
            SetShaderValue(shader, GetShaderLocation(shader, "cameraPosition"), &camera.position, SHADER_UNIFORM_VEC3);
            SetShaderValue(shader, GetShaderLocation(shader, "particleScale"), &pc.scale, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, GetShaderLocation(shader, "maxDrawDistance"), &this->draw_distance, SHADER_UNIFORM_FLOAT);
            // Apply bilboarding flag if renreing square markers 
            int billboarding = pc.marker_type == PointCloud::MarkerType::Square ? 1 : 0;
            SetShaderValue(shader, GetShaderLocation(shader, "billboarding"), &billboarding, SHADER_UNIFORM_INT);
            // Set the color mode (cast ot int)
            int colorMode = static_cast<int>(pc.coloring_mode);
            SetShaderValue(shader, GetShaderLocation(shader, "colorMode"), &colorMode, SHADER_UNIFORM_INT);
            // Pass the color
            Vector4 pointColor = { pc.color.r / 255.0f, pc.color.g / 255.0f, pc.color.b / 255.0f, pc.color.a / 255.0f };
            SetShaderValue(shader, GetShaderLocation(shader, "pointColor"), &pointColor, SHADER_UNIFORM_VEC4);
            // Bind the SSBO with point positions
            rlBindShaderBuffer(pc.ssboID, 0);

            // Draw the cubes using instanced rendering with the explicitly calculated index count
            rlDrawVertexArrayElementsInstanced(0, index_count, 0, pc.cloud->points.size());

            // Cleanup
            rlDisableVertexArray();
            rlDisableShader();
            rlEnableBackfaceCulling();
        }
    }
}