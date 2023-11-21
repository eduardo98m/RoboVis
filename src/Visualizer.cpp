#include "Visualizer.hpp"

Visualizer::Visualizer(int screen_width, int screen_height, const char* title) : screen_width_(screen_width), screen_height_(screen_height), title_(title) {
    InitWindow(screen_width_, screen_height_, title_);
    SetTargetFPS(60);
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

void Visualizer::update(){
    // Update the camera
    UpdateCamera(&camera_, CAMERA_THIRD_PERSON);

    // Draw
    BeginDrawing();
    // Clear the background
    ClearBackground(RAYWHITE);

    BeginMode3D(this->camera_);
    for (auto& vis_object : this->visual_objects_) {
        // Get the rotation axis and angle from the quaternion
        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(vis_object.orientation, &axis, &angle);
        DrawModelWiresEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, vis_object.color);
    }
    // Draw the axis
    DrawGrid(100, 1.0f);
    EndMode3D();

    DrawText("Orbit around the cube!", 10, 10, 20, DARKGRAY);

    
    EndDrawing();
}

void Visualizer::close() {
    // Unload all the models
    for (auto& vis_object : this->visual_objects_) {
        UnloadModel(vis_object.model);
    }
    CloseWindow();
}