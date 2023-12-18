
#include "Visualizer.hpp"
#include "raylib.h"
#include <iostream>
#include <algorithm> // std::clamp
#include <cmath> // std::tanh


int main() {
    // Initialize the visualizer
    Visualizer visualizer(1208, 720, "RoboVis");

    // Set up the camera
    visualizer.set_up_camera();

    Vector3 pos = {0.0f, -2.0f, 0.0f};
    Vector3 vel = {0.0f, 0.0f, 0.0f};
    // Create a box which will be the mass
    uint box_id =  visualizer.add_box(pos, QuaternionIdentity(), RED, 1.0, 1.0, 1.0);


    Vector3 ref_pos = {0.0f, 4.0f, 0.0f};
    // A reference point for the spring
    uint cylinder_id = visualizer.add_cylinder(ref_pos, QuaternionIdentity(), GREEN, 1.0f, 0.5f);

    Vector3 spring_pos  = Vector3Scale(Vector3Add(pos, ref_pos), 0.5f);
    // Now lets create a spring which will be a cylinder, the spring will be attached to the box and the reference point
    uint spring_id = visualizer.add_cylinder(spring_pos, QuaternionIdentity(), GRAY, 0.2, 1.0f);


    // Simulation parameters
    float k = 8.0f; // Spring constant
    float c = 0.2f; // Damping constant
    float m = 1.0f; // Mass
    float g = 9.81f; // Gravity
    float dt = 0.0167f; // Time step (same as the target FPS)
    float x_o = pos.y; // Initial position of the mass
    float v_o = vel.y; // Initial velocity of the mass

    // Equilibrium position of the spring
    float x_e = 2.0f;

    // Force of the spring
    float F_s = -k * (x_o - x_e);
    float F_d = -c * v_o;

    float poison_ratio = 0.3f;

    while (!WindowShouldClose()) {

        // Calculate the acceleration
        float a = (F_s - m * g + F_d) / m;
        // Update the velocity
        vel.y = vel.y + a * dt;
        // Update the position
        pos.y =  pos.y + vel.y * dt;

        // Update the force of the spring
        F_s = -k * (pos.y - x_e);
        F_d = -c * vel.y;

        // We need to update the position of the spring and its scale
        // The position of the spring is the average of the position of the mass and the reference point
        spring_pos.y = pos.y; //(pos.y + ref_pos.y) / 2.0f;
        // The scale of the spring is the distance between the mass and the reference point
        float elongation_y = Vector3Distance(pos, ref_pos);
        float elongation_x = elongation_y / poison_ratio;
        float elongation_z = elongation_y / poison_ratio;
        // clamp the elongation in the x and z axis using the tangent hyperbolic function, use as equilibirum position 1.0f value
        elongation_x = std::clamp(elongation_x, 0.0f, 2.0f);
        elongation_z = std::clamp(elongation_z, 0.0f, 2.0f);
        elongation_x = std::tanh(elongation_x);
        elongation_z = std::tanh(elongation_z);
        Vector3 spring_scale = {elongation_x, elongation_y, elongation_z};
        // Update the position and scale of the spring
        visualizer.update_visual_object_position_orientation_scale(spring_id, spring_pos, QuaternionIdentity(), spring_scale);

        // Update the position of the mass
        visualizer.update_visual_object_position_orientation(box_id, pos, QuaternionIdentity());

        // Update the visualizer
        visualizer.update();

    }
    // close the visualizer
    visualizer.close();
    
}