// #include "rlImGui.h"	
// #include "imgui.h"
#include "Visualizer.hpp"
// #define RAYGUI_IMPLEMENTATION
// #include"raygui.h"

Visualizer::Visualizer(int screen_width, int screen_height, const char* title) : 
    screen_width_(screen_width), 
    screen_height_(screen_height), 
    title_(title),
    wireframe_mode_(false), 
    focused_object_index_(-1)
    {
    InitWindow(screen_width_, screen_height_, title_);
    
    SetTargetFPS(60);
    rlImGuiSetup(true); // Setup ImGui
}

Visualizer::~Visualizer() {
    this->close();
}
void Visualizer::set_up_camera() {
    this->camera_ = { { 0.0f, 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE };
}

uint Visualizer::add_visual_object(VisualObject vis_object) {
    this->visual_objects_.push_back(vis_object);

    return this->visual_objects_.size() - 1;
}

void Visualizer::update_visual_object_position_orientation(uint index, Vector3 position, Quaternion orientation) {
    this->visual_objects_[index].position = position;
    this->visual_objects_[index].orientation = orientation;
}

void Visualizer::remove_visual_object(uint index) {
    this->visual_objects_.erase(this->visual_objects_.begin() + index);
}

void Visualizer::clear_visual_objects() {
    this->visual_objects_.clear();
}

void Visualizer::draw_shader() {
    BeginShaderMode(this->shader_);
    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
    // TODO: Fix this with the camera target

   // DrawTextureRec(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Vector2){ 0, 0 }, WHITE);
    EndShaderMode();
}

void Visualizer::draw_gui() {
    rlImGuiBegin();

    // Draw the GUI
    ImGui::Begin("Controls");
    ImGui::Text("Visualizer");
    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::Separator();
    ImGui::Text("Position: (%f, %f, %f)", this->camera_.position.x, this->camera_.position.y, this->camera_.position.z);
    ImGui::Text("Target: (%f, %f, %f)", this->camera_.target.x, this->camera_.target.y, this->camera_.target.z);
    ImGui::Text("Up: (%f, %f, %f)", this->camera_.up.x, this->camera_.up.y, this->camera_.up.z);
    ImGui::Text("FOV: %f", this->camera_.fovy);
    ImGui::Separator();
    ImGui::Text("Visual Objects");
    ImGui::Separator();
    ImGui::Text("Wireframe mode");
    ImGui::Checkbox("Wireframe mode", &this->wireframe_mode_);
    ImGui::Separator();
    ImGui::Text("Focused object");
    ImGui::InputInt("Focused object index", &this->focused_object_index_);
    // End the GUI
    ImGui::End();


     
}

void Visualizer::update(){

    // Update the camera

    this->draw_gui();
	
    UpdateCamera(&camera_, CAMERA_ORBITAL);

    // Draw
    BeginDrawing();
    // Clear the background
    ClearBackground({ 30, 30, 30, 255 } );

    BeginMode3D(this->camera_);
    for (auto& vis_object : this->visual_objects_) {
        // Get the rotation axis and angle from the quaternion
        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(vis_object.orientation, &axis, &angle);
        if (wireframe_mode_) {
            DrawModelEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, vis_object.color);
        } else {
            DrawModelWiresEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, vis_object.color);
        }
        
    }
    // Draw the axis
    DrawGrid(100, 1.0f);
    EndMode3D();  

    // Draw the GUI
    rlImGuiEnd();  
    EndDrawing();
    
}

// Functions to draw geometric primitives
uint Visualizer::add_cube(Vector3 position, Quaternion orientation, Color color, float width, float height, float length) {
    Mesh cube_mesh = GenMeshCube(width, height, length);
    Model cube = LoadModelFromMesh(cube_mesh);
    VisualObject cube_vis_object = { 
        position, 
        orientation, cube, color };
    
    return this->add_visual_object(cube_vis_object);
}

uint Visualizer::add_sphere(Vector3 position, Quaternion orientation, Color color, float radius) {
    Mesh sphere_mesh = GenMeshSphere(radius, 16, 16);
    Model sphere = LoadModelFromMesh(sphere_mesh);
    VisualObject sphere_vis_object = { 
        position, 
        orientation, sphere, color };
    
    return this->add_visual_object(sphere_vis_object);
}

uint Visualizer::add_cylinder(Vector3 position, Quaternion orientation, Color color, float radius, float height) {
    Mesh cylinder_mesh = GenMeshCylinder(radius, height, 16);
    Model cylinder = LoadModelFromMesh(cylinder_mesh);
    VisualObject cylinder_vis_object = { 
        position, 
        orientation, cylinder, color };
    
    return this->add_visual_object(cylinder_vis_object);
}

uint Visualizer::add_cone(Vector3 position, Quaternion orientation, Color color, float radius, float height) {
    Mesh cone_mesh = GenMeshCone(radius, height, 16);
    Model cone = LoadModelFromMesh(cone_mesh);
    VisualObject cone_vis_object = { 
        position, 
        orientation, cone, color };
    
    return this->add_visual_object(cone_vis_object);
}

uint Visualizer::add_plane(Vector3 position, Quaternion orientation, Color color, float width, float length) {
    Mesh plane_mesh = GenMeshPlane(width, length, 16, 16);
    Model plane = LoadModelFromMesh(plane_mesh);
    VisualObject plane_vis_object = { 
        position, 
        orientation, plane, color };
    
    return this->add_visual_object(plane_vis_object);
}

// uint Visualizer::add_heightmap(Vector3 position, Quaternion orientation, Color color, std::vector<std::vector<float>> heightmap) {
//     Mesh heightmap_mesh = GenMeshHeightmap(heightmap, 1.0f, 1.0f, 1.0f);
//     Model heightmap_model = LoadModelFromMesh(heightmap_mesh);
//     VisualObject heightmap_vis_object = { 
//         position, 
//         orientation, heightmap_model, color };
    
//     return this->add_visual_object(heightmap_vis_object);
// }


void Visualizer::close() {
    // Unload all the models
    for (auto& vis_object : this->visual_objects_) {
        UnloadModel(vis_object.model);
    }
    rlImGuiShutdown();
    CloseWindow();
}