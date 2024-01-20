#include "Visualizer.hpp"

Visualizer::Visualizer(int screen_width, int screen_height, const char *title) : screen_width_(screen_width),
                                                                                 screen_height_(screen_height),
                                                                                 title_(title),
                                                                                 wireframe_mode_(false),
                                                                                 focused_object_index_(0)
{
    InitWindow(screen_width_, screen_height_, title_);

    SetTargetFPS(60);
    rlImGuiSetup(true); // Setup ImGui
    this->set_up_camera();
    this->shader_target_ = LoadRenderTexture(screen_width_, screen_height_);
}

Visualizer::~Visualizer()
{
    this->close();
}
void Visualizer::set_up_camera()
{
    this->camera_ = (Camera3D){{0.0f, 10.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};
}

void Visualizer::set_camera_focus()
{
    this->focus_mode_ = (this->previously_focused_object_index_ != this->focused_object_index_ || this->focus_mode_);
    if (this->focus_mode_)
    {
        this->camera_.target = this->visual_objects_[this->focused_object_index_].position;
        this->previously_focused_object_index_ = this->focused_object_index_;
        this->focus_mode_ = true;
    }
}

int Visualizer::add_visual_object(VisualObject vis_object)
{
    this->visual_objects_.push_back(vis_object);

    return this->visual_objects_.size() - 1;
}

void Visualizer::update_visual_object_position_orientation(int index, Vector3 position, Quaternion orientation)
{
    this->visual_objects_[index].position = position;
    this->visual_objects_[index].orientation = orientation;
}

void Visualizer::update_visual_object_position_orientation_scale(int index, Vector3 position, Quaternion orientation, Vector3 scale)
{
    this->visual_objects_[index].position = position;
    this->visual_objects_[index].orientation = orientation;
    this->visual_objects_[index].model.transform = MatrixScale(scale.x, scale.y, scale.z);
}

void Visualizer::update_visual_object_scale(int index, Vector3 scale)
{
    this->visual_objects_[index].model.transform = MatrixScale(scale.x, scale.y, scale.z);
}

void Visualizer::remove_visual_object(int index)
{
    this->visual_objects_.erase(this->visual_objects_.begin() + index);
}

void Visualizer::clear_visual_objects()
{
    this->visual_objects_.clear();
}

void Visualizer::draw_shader()
{
    if (this->shader_loaded_)
    {
        BeginShaderMode(this->shader_);
        // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
        // TODO: Fix this with the camera target

        DrawTextureRec(this->shader_target_.texture,
                       (Rectangle){0, 0, (float)this->shader_target_.texture.width, (float)-this->shader_target_.texture.height},
                       (Vector2){0, 0}, WHITE);

        EndShaderMode();
    }
    else
    {
        DrawTextureRec(this->shader_target_.texture,
                       (Rectangle){0, 0, (float)this->shader_target_.texture.width, (float)-this->shader_target_.texture.height},
                       (Vector2){0, 0}, WHITE);
    }
}

void Visualizer::draw_gui()
{
    rlImGuiBegin();
    this->imgui_interfaces_calls();
    // Draw the GUI
    ImGui::Begin("Controls");
    ImGui::Text("Visualizer");
    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::Separator();
    ImGui::Text("Position: (%f, %f, %f)", this->camera_.position.x, this->camera_.position.y, this->camera_.position.z);
    ImGui::Text("Target: (%f, %f, %f)", this->camera_.target.x, this->camera_.target.y, this->camera_.target.z);
    ImGui::Text("Up: (%f, %f, %f)", this->camera_.up.x, this->camera_.up.y, this->camera_.up.z);
    ImGui::SliderFloat("FOV", &this->camera_.fovy, 6, 120);
    ImGui::Separator();
    ImGui::Text("Visual Objects");
    ImGui::Separator();
    ImGui::Text("Wireframe mode");
    ImGui::Checkbox("Wireframe mode", &this->wireframe_mode_);
    ImGui::Checkbox("Show Frames", &this->show_bodies_coordinate_frame_);
    ImGui::Separator();
    // ImGui::Text("Focused object");
    // ImGui::InputInt("Focused object index", &this->focused_object_index_);
    // Create a drop down menu for the focused object
    if (ImGui::BeginCombo("Focused object", TextFormat("Object %d", this->focused_object_index_)))
    {
        for (int n = 0; n < this->visual_objects_.size(); n++)
        {
            bool is_selected = (this->focused_object_index_ == n);

            if (ImGui::Selectable(TextFormat("Object %d", n), is_selected))
            {
                this->focused_object_index_ = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    // End the GUI
    ImGui::End();
    rlImGuiEnd();
}

void Visualizer::update_camera()
{
    // Camera movement
    Vector3 cameraFront = Vector3Subtract(this->camera_.target, this->camera_.position);
    Vector3 cameraRight = Vector3Normalize(Vector3CrossProduct(cameraFront, this->camera_.up));

    float cameraSpeed = 0.5f;
    float cameraRotationSpeed = 0.2f;

    Vector3 movement = {0.0f, 0.0f, 0.0f};
    Vector3 rotation = {0.0f, 0.0f, 0.0f};

    if (IsKeyDown(KEY_W))
        movement.x += cameraSpeed;
    if (IsKeyDown(KEY_S))
        movement.x -= cameraSpeed;
    if (IsKeyDown(KEY_A))
        movement.y -= cameraSpeed;
    if (IsKeyDown(KEY_D))
        movement.y += cameraSpeed;

    // Vertical movement with Shift (up) and Control (down) keys
    if (IsKeyDown(KEY_LEFT_SHIFT))
        movement.z += cameraSpeed;
    if (IsKeyDown(KEY_LEFT_CONTROL))
        movement.z -= cameraSpeed;
    if (IsKeyDown(KEY_Q))
        movement.z += cameraSpeed;
    if (IsKeyDown(KEY_E))
        movement.z -= cameraSpeed;

    // Camera rotation with the mouse
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        DisableCursor();
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
        EnableCursor();

    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        float sensitivity = 0.2f;
        rotation.y += (GetMouseY() - GetScreenHeight() / 2) * sensitivity;
        rotation.x += (GetMouseX() - GetScreenWidth() / 2) * sensitivity;

        // Limit the pitch to avoid flipping
        if (rotation.x > 89.0f)
            rotation.x = 89.0f;
        if (rotation.x < -89.0f)
            rotation.x = -89.0f;

        // Set the mouse position to the center of the screen
        // This is done to avoid the mouse leaving the window and to keep the movement smooth
        SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    }

    if (!Vector3Equals(rotation, {0.0, 0.0, 0.0}))
        this->focus_mode_ = false;

    // Update the camera
    UpdateCameraPro(&(this->camera_), movement, rotation, 0.0f);
}

void Visualizer::update()
{

    // Update the camera
    this->update_camera();
    this->set_camera_focus();
    this->select_visual_object();

    // Draw
    BeginTextureMode(this->shader_target_);
    // Clear the background
    ClearBackground({30, 30, 30, 255});
    // Draw the axis

    BeginMode3D(this->camera_);
    DrawGrid(100, 1.0f);
    EndMode3D();

    BeginMode3D(this->camera_);
    for (auto &vis_object : this->visual_objects_)
    {
        // Get the rotation axis and angle from the quaternion
        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(vis_object.orientation, &axis, &angle);
        angle = angle * 180 / 3.1415926535;
        if (wireframe_mode_)
        {
            DrawModelWiresEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, vis_object.color);
        }
        else
        {
            DrawModelEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, vis_object.color);
            // Draw wireframe with a small offset in the color (make it darker)
            Color wireColor = {(unsigned char)(vis_object.color.r * 0.9f),
                               (unsigned char)(vis_object.color.g * 0.9f),
                               (unsigned char)(vis_object.color.b * 0.9f), 255};
            DrawModelWiresEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, wireColor);
        }

        if (this->show_bodies_coordinate_frame_)
        {
            du::draw_axes(vis_object.position, vis_object.orientation);
        }
    }

    // Draw The lines
    while (!this->lines_.empty())
    {
        Line line = this->lines_.front();
        DrawLine3D(line.start_pos, line.end_pos, line.color);
        this->lines_.pop();
    }
    // Draw Arrows
    while (!this->arrows_.empty())
    {
        Arrow arrow = this->arrows_.front();
        du::draw_arrow(arrow.origin, Vector3Add(arrow.origin, arrow.vector), arrow.color, arrow.radius);
        this->arrows_.pop();
    }

    EndMode3D();

    // Draw the text on the normal labels
    for (const auto &[_, label] : this->text_labels_)
    {
        this->draw_text_label(label);
    }

    // Draw the text from the buffer
    while (!this->text_labels_buffer_.empty())
    {
        TextLabel label = this->text_labels_buffer_.front();
        this->draw_text_label(label);
        this->text_labels_buffer_.pop();
    }
    EndTextureMode();

    BeginDrawing();
    this->draw_shader();
    // Draw the GUI
    this->draw_gui();
    EndDrawing();
}

void Visualizer::draw_text_label(TextLabel label)
{
    float distance = Vector3Distance(label.position, this->camera_.position);
    Vector2 screenPosition = GetWorldToScreen(label.position, this->camera_);

    // Adjust text size based on the distance
    float text_size = (label.fontSize / distance);

    Vector2 text_dim = MeasureTextEx(label.font, label.text.c_str(), text_size, text_size * 0.3);

    Vector2 background_pos = screenPosition;
    background_pos.x = screenPosition.x - text_dim.x * 0.05;
    background_pos.y = screenPosition.y - text_dim.y * 0.05;
    text_dim.x = text_dim.x * 1.10;
    text_dim.y = text_dim.y * 1.10;

    DrawRectangleV(background_pos, text_dim, BLACK);
    DrawTextEx(label.font, label.text.c_str(), screenPosition, text_size, text_size * 0.3, WHITE);
}

void Visualizer::draw_text(std::string text, Vector3 position, float font_size, bool background, Color color, Font font, Color background_color)
{
    TextLabel label = {
        text,
        position,
        font_size,
        color,
        font,
        background,
        background_color,
        true};

    this->text_labels_buffer_.push(label);
}

int Visualizer::add_text_label(std::string text, Vector3 position, float font_size, bool background, Color color, Font font, Color background_color)
{
    TextLabel label = {
        text,
        position,
        font_size,
        color,
        font,
        background,
        background_color,
        true};

    this->text_labels_.insert({this->text_labels_.size() - 1, label});

    return this->text_labels_.size() - 1;
}

void Visualizer::modify_text_label(int index, std::string text)
{
    this->text_labels_[index].text = text;
}

void Visualizer::modify_text_position(int index, Vector3 position)
{
    this->text_labels_[index].position = position;
}

// Functions to draw geometric primitives
int Visualizer::add_box(Vector3 position, Quaternion orientation, Color color, float width, float height, float length)
{
    Mesh cube_mesh = GenMeshCube(width, height, length);
    Model cube = LoadModelFromMesh(cube_mesh);
    VisualObject cube_vis_object = {
        position,
        orientation, cube, color};

    return this->add_visual_object(cube_vis_object);
}

int Visualizer::add_sphere(Vector3 position, Quaternion orientation, Color color, float radius)
{
    Mesh sphere_mesh = GenMeshSphere(radius, 16, 16);
    Model sphere = LoadModelFromMesh(sphere_mesh);
    VisualObject sphere_vis_object = {
        position,
        orientation, sphere, color};

    return this->add_visual_object(sphere_vis_object);
}

int Visualizer::add_cylinder(Vector3 position, Quaternion orientation, Color color, float radius, float height)
{
    Mesh cylinder_mesh = GenMeshCylinder(radius, height, 16);
    Model cylinder = LoadModelFromMesh(cylinder_mesh);
    VisualObject cylinder_vis_object = {
        position,
        orientation, cylinder, color};

    return this->add_visual_object(cylinder_vis_object);
}

int Visualizer::add_cone(Vector3 position, Quaternion orientation, Color color, float radius, float height)
{
    Mesh cone_mesh = GenMeshCone(radius, height, 16);
    Model cone = LoadModelFromMesh(cone_mesh);
    VisualObject cone_vis_object = {
        position,
        orientation, cone, color};

    return this->add_visual_object(cone_vis_object);
}

int Visualizer::add_plane(Vector3 position, Quaternion orientation, Color color, float width, float length)
{
    Mesh plane_mesh = GenMeshPlane(width, length, 16, 16);
    Model plane = LoadModelFromMesh(plane_mesh);
    VisualObject plane_vis_object = {
        position,
        orientation, plane, color};

    return this->add_visual_object(plane_vis_object);
}

int Visualizer::add_mesh(const char *filename, Vector3 position, Quaternion orientation, Color color, float scale)
{
    Model model = LoadModel(filename);
    model.transform = MatrixScale(scale, scale, scale);
    VisualObject vis_object = {
        position,
        orientation, model, color};

    return this->add_visual_object(vis_object);
}

void Visualizer::draw_line(Vector3 start_pos, Vector3 end_pos, Color color)
{
    Line line = {
        start_pos,
        end_pos,
        color};
    this->lines_.push(line);
}

void Visualizer::draw_arrow(Vector3 origin, Vector3 vector, float radius, Color color)
{
    Arrow arrow = {
        origin,
        vector,
        radius,
        color};
    this->arrows_.push(arrow);
}

// int Visualizer::add_heightmap(Vector3 position, Quaternion orientation, Color color, std::vector<std::vector<float>> heightmap) {
//     Mesh heightmap_mesh = GenMeshHeightmap(heightmap, 1.0f, 1.0f, 1.0f);
//     Model heightmap_model = LoadModelFromMesh(heightmap_mesh);
//     VisualObject heightmap_vis_object = {
//         position,
//         orientation, heightmap_model, color };

//     return this->add_visual_object(heightmap_vis_object);
// }
void Visualizer::set_up_lighting()
{
    this->light_ = CreateLight(LIGHT_DIRECTIONAL, (Vector3){0.0f, 3.0f, 0.0f}, Vector3Zero(), WHITE, this->shader_);

    this->shader_.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(this->shader_, "viewPos");

    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(this->shader_, "ambient");
    Vector4 ambient = {1.0f, 1.0f, 1.0f, 1.0f}; // Use Vector4 for ambient light
    SetShaderValue(this->shader_, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);

    // Assign out lighting shader to model

    for (auto &vis_object : this->visual_objects_)
    {
        vis_object.model.materials[0].shader = this->shader_;
    }
}

void Visualizer::load_shader(const char *filename_vs, const char *filename_fs, int group_id)
{
    this->shader_ = LoadShader(TextFormat(filename_vs, GLSL_VERSION), TextFormat(filename_fs, GLSL_VERSION));
    this->shader_loaded_ = true;
}

void Visualizer::close()
{
    // Unload all the models
    for (auto &vis_object : this->visual_objects_)
    {
        UnloadModel(vis_object.model);
    }
    rlImGuiShutdown();
    UnloadRenderTexture(this->shader_target_);
    CloseWindow();
}

void Visualizer::set_imgui_interfaces(std::function<void(void)> func)
{
    this->imgui_interfaces_calls = func;
}

int Visualizer::select_visual_object()
{
    // Static variables to track the double click
    static bool double_click = true;
    static float double_click_time = 0.0f;

    float nearest_collision_distance = FLT_MAX;

    // Apply the function if there is a double click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && double_click)
    {

        double_click = false;
        Ray ray = GetMouseRay(GetMousePosition(), this->camera_);
        DrawRay(ray, GREEN);
        int i = 0;
        for (const auto &obj : this->visual_objects_)
        {
            // Note that this works because we use single mesh models.
            RayCollision collision = GetRayCollisionMesh(ray, obj.model.meshes[0], du::get_transform(obj.position, obj.orientation));

            if (collision.hit && collision.distance < nearest_collision_distance)
            {
                // Update the nearest object information
                nearest_collision_distance = collision.distance;
                this->focused_object_index_ = i;
            }
            i++;
        }
    }
    // If there is a single click start the double click timer
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !double_click)
    {
        double_click = true;
        double_click_time = GetTime();
    }
    // Reset double-click flag after a certain time threshold (0.5s)
    if (double_click && (GetTime() - double_click_time > 0.5)) 
    {
        double_click = false;
    }
    // Finally return the object at focuss
    return this->focused_object_index_;
}
