#include "raylib.h"
#include "VisualizerNew.hpp"
#include "rlights.hpp"
#include <memory>
#include <optional>
#include <vector>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <stdio.h>


void gui_test(){
    ImGui::Begin("Hello!");
        ImGui::Text("I'm Testing gui registration");
    ImGui::End();
}

int main()
{
    // Initialize the visualizer
    rbvs::Visualizer visualizer = rbvs::Visualizer(1208, 720, "RoboVis");

    // Create a simple point cloud
    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);

    float step = 0.5;
    float lenght = 10 * step;

    for (float x = -lenght; x <= lenght; x += step)
    {
        for (float y = -lenght; y <= lenght; y += step)
        {
            for (float z = -lenght; z <= lenght; z += step)
            {
                cloud->points.push_back(pcl::PointXYZRGBA(x, y, z, 255, 0, 255, 255));
                
            }
        }
    }


    visualizer.add_gui("test gui", gui_test);

    

    // 8 120 601 : 100
    // 13 997 521 : 120

    float x;
    std::cout << "This is the number of points for the rendering: " << cloud->points.size() << "\n";
    // std::cin >> x;

    // Create a point cloud entity
    rbvs::Entity point_cloud_entity = visualizer.create_point_cloud<pcl::PointXYZRGBA>(
        rbvs::PointCloudParams<pcl::PointXYZRGBA>{
            .position = {10.0, 10.0, 0.0},
            .orientation = QuaternionIdentity(),
            .color = RED,
            .scale = 0.05,
            .cloud = cloud,
            .marker_type = rbvs::PointCloudStyle::Square});

    size_t n_x = 200;
    size_t n_y = 200;
    std::vector<float> heights(n_x * n_y, 0.0f);
    std::vector<Color> color_map(n_x * n_y, WHITE);


    for (int i = 0; i < n_x; i++)
    {
        for (int j = 0; j <= n_y; j++)
        {
            float x = (i > j) ? (0.1) : (0.2);
            Color c = (i % 2) ? (Color){.r = 0, .g = 255, .b = 58, .a = 75} : (Color){.r = 255, .g = 0, .b = 58, .a = 75};
            heights[i * n_x + j] = x;
            color_map[i * n_x + j] = c;
        }
    }

    std::vector<float> heights_b(n_x * n_y, 0.0f);

    std::vector<Color> color_map_b(n_x * n_y, WHITE);

    for (int i = 0; i < n_x; i++)
    {
        for (int j = 0; j <= n_y; j++)
        {
            float x = (i > j) ? (0.2) : (0.1);
            Color c = (i % 2) ? (Color){.r = 125, .g = 30, .b = 58, .a = 75} : (Color){.r = 0, .g = 120, .b = 58, .a = 75};
            heights_b[i * n_x + j] = x;
            color_map_b[i * n_x + j] = c;
        }
    }



    rbvs::Entity height_map = visualizer.create_height_map(
        rbvs::HeightMapParams{
            .position = {0.0, 5.0, 0.0},
            .orientation = QuaternionFromEuler(0.0, 2.0, 0.0),
            .scale = {100.0, 1.0, 100.0},
            .heights = heights,
            .n_x = n_x,
            .n_y = n_y,
            .color = {.r = 0, .g = 255, .b = 58, .a = 75},
            });

    // for ();

    rbvs::Entity cylinder_entity = visualizer.create_model(
        rbvs::ModelParams{
            .position = {1.0, 0.0, 1.0},
            .color = {255, 0, 0, 255},
            .model_type = rbvs::ModelType::CYLINDER,
            .radius = 0.1,
            .length = 5.0,
        });

    visualizer.create_model(
        rbvs::ModelParams{
            .position = {1.0, 0.0, -1.0},
            .color = {255, 0, 0, 255},
            .model_type = rbvs::ModelType::CYLINDER,
            .radius = 0.1,
            .length = 5.0,
        });

    visualizer.create_model(
        rbvs::ModelParams{
            .position = {-1.0, 0.0, -1.0},
            .color = {255, 0, 0, 255},
            .model_type = rbvs::ModelType::CYLINDER,
            .radius = 0.1,
            .length = 5.0,
        });

    visualizer.create_model(
        rbvs::ModelParams{
            .position = {-1.0, 0.0, 1.0},
            .color = {255, 0, 0, 255},
            .model_type = rbvs::ModelType::CYLINDER,
            .radius = 0.1,
            .length = 5.0,
        });

    rbvs::Entity cone_entity = visualizer.create_model(
        rbvs::ModelParams{
            .position = {4.0, 0.0, 2.0},
            .color = {255, 255, 0, 50},
            .model_type = rbvs::ModelType::CONE,
            .radius = 1.0,
            .length = 5.0,
        });

    rbvs::Entity cone_entity_2 = visualizer.create_model(
        rbvs::ModelParams{
            .position = {4.0, 0.0, -2.0},
            .color = {100, 0, 255, 50},
            .model_type = rbvs::ModelType::CONE,
            .radius = 1.0,
            .length = 3.0,
        });

    rbvs::Entity gizmo = visualizer.create_gizmo(
        rbvs::GizmoParams{
            .position = {0.0, 0.0, -2.0},
        });

    // Define the sphere
    Vector3 new_pos = {0.0f, 0.0f, 0.0f};
    Vector3 new_scale = {1.0f, 1.0f, 1.0f};
    Quaternion new_orientation = QuaternionIdentity();
    // uint cube_id = visualizer.add_box(new_pos, QuaternionIdentity(), RED, 1.0, 1.0, 1.0);
    SetTargetFPS(60);
    // Main game loop
    float t_o = GetTime();
    bool latch = false;
    while (!WindowShouldClose())
    {

        // Update the visualizer
        visualizer.update();

        float floatHeight = 2.0f;
        float rotationSpeed = 2.0f;

        new_pos.y = floatHeight * sinf(GetTime());
        new_scale.x = 0.3 + sinf(GetTime());
        new_orientation = QuaternionFromEuler(0.0f, GetTime() * rotationSpeed, 0.0f);

        // visualizer.update_point_cloud(rbvs::PointCloudUpdateParams{
        //     .entity = point_cloud_entity,
        //     .position = std::optional<Vector3>{new_pos},
        // });

        // if ((GetTime() - t_o) > 0.5)
        // {
        //     latch = !latch;
        //     auto &chosen = latch ? heights : heights_b;
        //     auto &chosen_c_map = latch ? color_map : color_map_b;
        //     Color c =  latch ? (Color){.r = 0, .g = 255, .b = 58, .a = 75} : (Color){.r = 255, .g = 0, .b = 58, .a = 75};
        //     visualizer.update_height_map(
        //         rbvs::HeightMapUpdateParams{
        //             .entity = height_map,
        //             .position = std::optional<Vector3>{new_pos},
        //             .heights = chosen,
        //             .n_x = n_x,
        //             .n_y = n_y,
        //             .color = c,
        //             .color_map = chosen_c_map});
        //     t_o = GetTime();
        // }
        // else{
        //     visualizer.update_height_map(
        //         rbvs::HeightMapUpdateParams{
        //             .entity = height_map,
        //             .position = std::optional<Vector3>{new_pos},
        //             });
        // }

        // visualizer.update_model(rbvs::ModelUpdateParams{
        //     .entity = sphere_entity,
        //     .position = std::optional<Vector3>{new_pos},  // <-- semicolon here is invalid
        //     .scale = std::optional<Vector3>{new_scale}
        // });

        // if ((GetTime() - t_o) > 5.0) {
        //     // Delete the box
        //     visualizer.delete_model(cube_entity);
        // }

        // visualizer.update_visual_object_position_orientation(cube_id, new_pos, new_orientation);
    }

    // De-initialize
    CloseWindow();

    return 0;
}
