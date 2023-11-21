#include "raylib.h"
#include "Visualizer.hpp"

int main() {
    // Initialize the visualizer
    Visualizer visualizer(800, 600, "RoboVis");

    // Set up the camera
    visualizer.set_up_camera();

    // Define the cube
    Mesh cube_mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model cube = LoadModelFromMesh(cube_mesh);
    VisualObject cube_vis_object = { 
        { 0.0f, 0.0f, 0.0f }, 
        QuaternionIdentity(), cube, RED };
    
    // Define the sphere
    Mesh sphere_mesh = GenMeshSphere(1.0f, 16, 16);
    Model sphere = LoadModelFromMesh(sphere_mesh);
    VisualObject sphere_vis_object = { 
        { 0.0f, 2.0f, 0.0f }, 
        QuaternionIdentity(), sphere, BLUE };
    // Add the visual objects
    uint cube_id = visualizer.add_visual_object(cube_vis_object);
    uint sphere_id = visualizer.add_visual_object(sphere_vis_object);
    Vector3 new_pos = { 0.0f, 2.0f, 0.0f };
    Quaternion new_orientation = QuaternionIdentity();
    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose()) {

        // Update the visualizer
        visualizer.update();

        float floatHeight = 2.0f;
        float rotationSpeed = 2.0f;

        new_pos.y = floatHeight * sinf(GetTime());
        new_orientation = QuaternionFromEuler(0.0f, GetTime() * rotationSpeed, 0.0f);

        visualizer.update_visual_object_position_orientation(cube_id, new_pos, new_orientation);

    }

    // De-initialize
    CloseWindow();

    return 0;
}
