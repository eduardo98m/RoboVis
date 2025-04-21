#include "Systems/Rendering/PointCloudRendering.hpp"

namespace rbvs
{
    void PointCloudRenderingSystem::load_shader()
    {
        std::string vs_path = std::string(SHADER_BASE_PATH) + "/pointcloud.vs";
        std::string fs_path = std::string(SHADER_BASE_PATH) + "/pointcloud.fs";

        this->shader = LoadShader(TextFormat(vs_path.c_str(), GLSL_VER), TextFormat(fs_path.c_str(), GLSL_VER));

        vs_path = std::string(SHADER_BASE_PATH) + "/pointcloud_colored.vs";
        fs_path = std::string(SHADER_BASE_PATH) + "/pointcloud.fs";

        this->color_pc_shader = LoadShader(TextFormat(vs_path.c_str(), GLSL_VER), TextFormat(fs_path.c_str(), GLSL_VER));
    }

    void PointCloudRenderingSystem::update_shader_values(Shader &shdr,
                                                         const Camera &camera,
                                                         float scale,
                                                         const Matrix &model,
                                                         const Matrix &view,
                                                         const Matrix &projection,
                                                         const PointCloudStyle::MarkerType &marker_type,
                                                         const PointCloudStyle::ColoringMode &coloring_mode,
                                                         const Color &point_color)
    {

        Vector3 view_right = Vector3{view.m0, view.m4, view.m8};
        Vector3 view_up = Vector3{view.m1, view.m5, view.m9};
        // Set shader uniforms
        SetShaderValueMatrix(shdr, GetShaderLocation(shdr, "projectionMatrix"), projection);
        SetShaderValueMatrix(shdr, GetShaderLocation(shdr, "viewMatrix"), view);
        SetShaderValueMatrix(shdr, GetShaderLocation(shdr, "modelMatrix"), model);
        SetShaderValue(shdr, GetShaderLocation(shdr, "CameraRight_worldspace"), &view_right, SHADER_UNIFORM_VEC3);
        SetShaderValue(shdr, GetShaderLocation(shdr, "CameraUp_worldspace"), &view_up, SHADER_UNIFORM_VEC3);
        SetShaderValue(shdr, GetShaderLocation(shdr, "cameraPosition"), &camera.position, SHADER_UNIFORM_VEC3);
        SetShaderValue(shdr, GetShaderLocation(shdr, "particleScale"), &scale, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shdr, GetShaderLocation(shdr, "maxDrawDistance"), &this->draw_distance, SHADER_UNIFORM_FLOAT);
        // Apply bilboarding flag if renreing square markers
        int billboarding = marker_type == PointCloudStyle::MarkerType::Square ? 1 : 0;
        SetShaderValue(shdr, GetShaderLocation(shdr, "billboarding"), &billboarding, SHADER_UNIFORM_INT);
        // Set the color mode (cast ot int)
        int colorMode = static_cast<int>(coloring_mode);
        SetShaderValue(shdr, GetShaderLocation(shdr, "colorMode"), &colorMode, SHADER_UNIFORM_INT);
        // Pass the color
        Vector4 pointColor = {point_color.r / 255.0f, point_color.g / 255.0f, point_color.b / 255.0f, point_color.a / 255.0f};
        SetShaderValue(shdr, GetShaderLocation(shdr, "pointColor"), &pointColor, SHADER_UNIFORM_VEC4);
    }

    void PointCloudRenderingSystem::update(EntityManager &em, Camera3D camera)
    {
        auto view = em.view<PointCloud<pcl::PointXYZ>>();

        for (auto entity : view)
        {
            // Get the pointcloud entuty from the entity manager
            auto &pc = em.getComponent<PointCloud<pcl::PointXYZ>>(entity);

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
            if (pc.marker_type == PointCloudStyle::MarkerType::Square)
            {
                // Setup VBO for the cube vertices
                index_count = sizeof(square_indices) / sizeof(square_indices[0]);
                int vbo = rlLoadVertexBuffer(square_vertices, sizeof(square_vertices), false);
                int ibo = rlLoadVertexBufferElement(square_indices, sizeof(square_indices), false);
            }
            else
            {
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

            update_shader_values(shader, camera,
                                 pc.scale,
                                 model,
                                 view,
                                 projection,
                                 pc.marker_type,
                                 pc.coloring_mode,
                                 pc.color);
            rlBindShaderBuffer(pc.ssboID, 0);

            // Draw the cubes using instanced rendering with the explicitly calculated index count
            rlDrawVertexArrayElementsInstanced(0, index_count, 0, pc.cloud->points.size());

            // Cleanup
            rlDisableVertexArray();
            rlDisableShader();
            rlEnableBackfaceCulling();
        }

        auto view_color = em.view<PointCloud<pcl::PointXYZRGBA>>();

        for (auto entity : view_color)
        {
            // Get the pointcloud entuty from the entity manager
            auto &pc = em.getComponent<PointCloud<pcl::PointXYZRGBA>>(entity);

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
            // --- Prepare the VAO [Vertex array object] for instanced rendering ---
            int particleVao = rlLoadVertexArray();
            rlEnableVertexArray(particleVao);
            if (pc.ssboID == 0)
            {
                pc.ssboID = rlLoadShaderBuffer(pc.cloud->points.size() * sizeof(Vector4), pc.pos_buffer.data(), RL_DYNAMIC_COPY);
            }
            else
            {
                rlUpdateShaderBuffer(pc.ssboID, pc.pos_buffer.data(), pc.cloud->points.size() * sizeof(Vector4), 0);
            }

            if (pc.color_ssboID == 0)
            {
                pc.color_ssboID = rlLoadShaderBuffer(pc.cloud->points.size() * sizeof(Vector4), pc.color_buffer.data(), RL_DYNAMIC_COPY);
            }
            else
            {
                rlUpdateShaderBuffer(pc.color_ssboID, pc.color_buffer.data(), pc.cloud->points.size() * sizeof(Vector4), 0);
            }

            // Get the correct shape type markers
            int index_count;
            int vbo, ibo;
            if (pc.marker_type == PointCloudStyle::MarkerType::Square)
            {
                // Setup VBO for the cube vertices
                index_count = sizeof(square_indices) / sizeof(square_indices[0]);
                vbo = rlLoadVertexBuffer(square_vertices, sizeof(square_vertices), false);
                ibo = rlLoadVertexBufferElement(square_indices, sizeof(square_indices), false);
            }
            else
            {
                // Setup VBO for the cube vertices
                index_count = sizeof(cube_indices) / sizeof(cube_indices[0]);
                vbo = rlLoadVertexBuffer(cube_vertices, sizeof(cube_vertices), false);
                ibo = rlLoadVertexBufferElement(cube_indices, sizeof(cube_indices), false);
            }

            // Configure vertex attributes
            rlEnableVertexAttribute(0);
            rlSetVertexAttribute(0, 3, RL_FLOAT, false, 0, 0);
            rlDisableBackfaceCulling();

            // Start Drawing ---
            rlEnableShader(this->color_pc_shader.id);

            this->update_shader_values(this->color_pc_shader,
                                       camera,
                                       pc.scale,
                                       model,
                                       view,
                                       projection,
                                       pc.marker_type,
                                       pc.coloring_mode,
                                       pc.color);

            rlBindShaderBuffer(pc.ssboID, 0);
            rlBindShaderBuffer(pc.color_ssboID, 1);

            // Draw the cubes using instanced rendering with the explicitly calculated index count
            rlDrawVertexArrayElementsInstanced(0, index_count, 0, pc.cloud->points.size());

            // Cleanup
            rlDisableVertexArray();
            rlDisableShader();
            rlEnableBackfaceCulling();
        }
    }
}