#include "raylib.h"
#include "Visualizer.hpp"
#include "rlights.hpp"
#include <memory>
int main() {
    // Initialize the visualizer
    Visualizer visualizer(1208, 720, "RoboVis");

    // Set up the camera
    visualizer.set_up_camera();
    

    uint sphere_id = visualizer.add_sphere({0.0f, 2.0f, 0.0f}, QuaternionIdentity(), BLUE, 1.0f);
    uint cylinder_id = visualizer.add_cylinder({1.0f, 0.0f, 0.0f}, QuaternionIdentity(), GREEN, 1.0f, 2.0f);
    uint cone_id = visualizer.add_cone({-1.0f, 0.0f, 0.0f}, QuaternionIdentity(), YELLOW, 1.0f, 2.0f);
    uint plane = visualizer.add_plane({0.0f, -1.0f, 0.0f}, QuaternionIdentity(), WHITE, 10.0f, 10.0f); 

    uint monkey_id = visualizer.add_mesh("../resources/models/monkey.obj", {0.0f, 1.0f, 1.0f}, QuaternionIdentity(), RED, 1.0f);

    //visualizer.load_shader("../resources/shaders/lighting.vs" , "../resources/shaders/lighting.fs");
    //visualizer.set_up_lighting();
    
    // Define the sphere
    Vector3 new_pos = {0.0f, 0.0f, 0.0f};
    Quaternion new_orientation = QuaternionIdentity();
    uint cube_id = visualizer.add_box(new_pos, QuaternionIdentity(), RED, 1.0, 1.0, 1.0);
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
