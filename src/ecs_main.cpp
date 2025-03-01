#include "raylib.h"
#include "VisualizerNew.hpp"
#include "rlights.hpp"
#include <memory>
#include <optional>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <stdio.h>

int main() {
    // Initialize the visualizer
    rbvs::Visualizer visualizer = rbvs::Visualizer(1208, 720, "RoboVis");

    // Create a simple point cloud
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

    float step = 0.5;
    float lenght = 50 * step;

    for (float x = -lenght; x <= lenght; x += step) {
        for (float y = -lenght; y <= lenght; y += step) {
            for (float z = -lenght; z <= lenght; z += step) {
                cloud->points.push_back(pcl::PointXYZ(x, y, z));
            }
        }
    }

    float x;
    std::cout << "This is the number of points for the rendering: " << cloud->points.size() << "\n";
    std::cin >> x;

    // Create a point cloud entity
    rbvs::Entity point_cloud_entity = visualizer.create_point_cloud(
        rbvs::PointCloudParams{
            .position = {1.0, 1.0, 0.0},
            .orientation = QuaternionIdentity(),
            .color = RED,
            .cloud = cloud
        }
    );

    rbvs::Entity cylinder_entity = visualizer.create_model(
            rbvs::ModelParams{
                .position = {1.0, 0.0, 1.0},
                .color = {255, 0 , 0,  255},
                .model_type = rbvs::ModelType::CYLINDER,
                .radius = 0.1,
                .length = 5.0,
            }
        );

    visualizer.create_model(
            rbvs::ModelParams{
                .position = {1.0, 0.0, -1.0},
                .color = {255, 0 , 0,  255},
                .model_type = rbvs::ModelType::CYLINDER,
                .radius = 0.1,
                .length = 5.0,
            }
        );
    
        visualizer.create_model(
            rbvs::ModelParams{
                .position = {-1.0, 0.0, -1.0},
                .color = {255, 0 , 0,  255},
                .model_type = rbvs::ModelType::CYLINDER,
                .radius = 0.1,
                .length = 5.0,
            }
        );

        visualizer.create_model(
            rbvs::ModelParams{
                .position = {-1.0, 0.0, 1.0},
                .color = {255, 0 , 0,  255},
                .model_type = rbvs::ModelType::CYLINDER,
                .radius = 0.1,
                .length = 5.0,
            }
        );


    rbvs::Entity cone_entity = visualizer.create_model(
        rbvs::ModelParams{
            .position = {4.0, 0.0, 2.0},
            .color = {255, 255 , 0,  50},
            .model_type = rbvs::ModelType::CONE,
            .radius = 1.0,
            .length = 5.0,
        }
    );


    rbvs::Entity cone_entity_2 = visualizer.create_model(
        rbvs::ModelParams{
            .position = {4.0, 0.0, -2.0},
            .color = {100, 0 , 255,  50},
            .model_type = rbvs::ModelType::CONE,
            .radius = 1.0,
            .length = 3.0,
        }
    );

    
    // Define the sphere
    Vector3 new_pos = {0.0f, 0.0f, 0.0f};
    Vector3 new_scale = {1.0f, 1.0f, 1.0f};
    Quaternion new_orientation = QuaternionIdentity();
    //uint cube_id = visualizer.add_box(new_pos, QuaternionIdentity(), RED, 1.0, 1.0, 1.0);
    SetTargetFPS(60);
    // Main game loop
    float t_o = GetTime(); 
    while (!WindowShouldClose()) {

        // Update the visualizer
        visualizer.update();

        float floatHeight = 2.0f;
        float rotationSpeed = 2.0f;

        new_pos.y = floatHeight * sinf(GetTime());
        new_scale.x = 0.3 + sinf(GetTime());
        new_orientation = QuaternionFromEuler(0.0f, GetTime() * rotationSpeed, 0.0f);

        visualizer.update_point_cloud(rbvs::PointCloudUpdateParams{
            .entity = point_cloud_entity,
            .position = std::optional<Vector3>{new_pos},
        });

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
