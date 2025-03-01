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
            auto &pc = em.getComponent<PointCloud>(entity);
            if (!pc.visible || pc.cloud->points.empty())
                continue;

            // --- Compute Model Matrix ---
            Matrix translation = MatrixTranslate(pc.position.x, pc.position.y, pc.position.z);
            Matrix rotation = QuaternionToMatrix(pc.orientation);
            Matrix model = MatrixMultiply(rotation, translation);

            // --- Prepare the VAO for instanced rendering ---
            int particleVao = rlLoadVertexArray();
            rlEnableVertexArray(particleVao);

            if (pc.ssboID == 0)
            {
                pc.ssboID = rlLoadShaderBuffer(pc.cloud->points.size() * sizeof(pcl::PointXYZ), pc.cloud->points.data(), RL_DYNAMIC_COPY);
            }
            else
            {
                rlUpdateShaderBuffer(pc.ssboID, pc.cloud->points.data(), pc.cloud->points.size() * sizeof(pcl::PointXYZ), 0);
            }

            // --- Setup base geometry for instancing ---
            // We're creating a cube where each vertex is at a corner of a unit cube
            Vector3 vertices[] = {
                // Front face
                { -0.5f, -0.5f,  0.5f },
                {  0.5f, -0.5f,  0.5f },
                {  0.5f,  0.5f,  0.5f },
                { -0.5f,  0.5f,  0.5f },
                // Back face
                { -0.5f, -0.5f, -0.5f },
                {  0.5f, -0.5f, -0.5f },
                {  0.5f,  0.5f, -0.5f },
                { -0.5f,  0.5f, -0.5f }
            };

            unsigned short indices[] = {
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

            // Calculate number of indices
            int indexCount = sizeof(indices) / sizeof(indices[0]);

            // Setup VBO for the cube vertices
            int vbo = rlLoadVertexBuffer(vertices, sizeof(vertices), false);
            int ibo = rlLoadVertexBufferElement(indices, sizeof(indices), false);

            // Configure vertex attributes
            rlEnableVertexAttribute(0);
            rlSetVertexAttribute(0, 3, RL_FLOAT, false, 0, 0);
            rlDisableBackfaceCulling();
            // --- Drawing ---
            rlEnableShader(shader.id);

            // Set matrices
            Matrix projection = rlGetMatrixProjection();
            Matrix view = GetCameraMatrix(camera);

            // Set shader uniforms
            SetShaderValueMatrix(shader, GetShaderLocation(shader, "projectionMatrix"), projection);
            SetShaderValueMatrix(shader, GetShaderLocation(shader, "viewMatrix"), view);
            SetShaderValueMatrix(shader, GetShaderLocation(shader, "modelMatrix"), model);

            // Set particle scale
            float particleScale = pc.scale;  // Adjust this value as needed
            SetShaderValue(shader, GetShaderLocation(shader, "particleScale"), &particleScale, SHADER_UNIFORM_FLOAT);

            // Bind the SSBO with point positions
            rlBindShaderBuffer(pc.ssboID, 0);

            // Draw the cubes using instanced rendering with the explicitly calculated index count
            rlDrawVertexArrayElementsInstanced(0, indexCount, 0, pc.cloud->points.size());

            // Cleanup
            rlDisableVertexArray();
            rlDisableShader();
            rlEnableBackfaceCulling();

            // This doesnt seem necessary
            //rlUnloadVertexArray(particleVao);
            
            // We keep the SSBO for reuse
        }
    }
}