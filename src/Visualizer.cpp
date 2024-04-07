#include "Visualizer.hpp"

Visualizer::Visualizer(int screen_width, int screen_height, const char *title) : screen_width_(screen_width),
                                                                                 screen_height_(screen_height),
                                                                                 title_(title),
                                                                                 wireframe_mode_(false),
                                                                                 focused_object_index_(0)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width_, screen_height_, title_);

    SetTargetFPS(60);
    rlImGuiSetup(true); // Setup ImGui
    this->set_up_camera();
    this->shader_target_ = LoadRenderTexture(screen_width_, screen_height_);
    this->base_shader_ = LoadModelFromMesh(GenMeshTorus(.3f, 2.f, 20, 20)).materials[0].shader;
}

Visualizer::~Visualizer()
{
    this->close();
}
void Visualizer::set_up_camera()
{

    this->camera_ = Camera3D{
        .position = {0.0f, 10.0f, 10.0f},
        .target = {.0f, .0f, .0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE};

    this->shadow_map_camera = Camera3D{
        .position = {0.0f, 10.0f, 0.0f},
        .target = {.0f, .0f, .0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 20.0f,
        .projection = CAMERA_PERSPECTIVE};
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
    // Add the shader to the visual object (if the shader is loaded)
    if (this->shader_loaded_)
    {
        vis_object.model.materials[0].shader = this->shaders_["light"];
    }
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

void Visualizer::clear_gui_interfaces()
{
    this->imgui_interfaces_calls.clear();
}

void Visualizer::draw_shader()
{
    if (this->shader_loaded_)
    {
        // BeginShaderMode(this->shaders_["light"]);
        //   NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
        //   TODO: Fix this with the camera target

        DrawTextureRec(this->shader_target_.texture,
                       (Rectangle){0, 0, (float)this->shader_target_.texture.width, (float)-this->shader_target_.texture.height},
                       (Vector2){0, 0}, WHITE);

        // EndShaderMode();
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

    for (auto &imgui_interface_function : this->imgui_interfaces_calls)
    {
        imgui_interface_function();
    }
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

    // Update Camera Looking Vector. Vector length determines FOV.
    this->shadow_map_camera.position = this->camera_.position;

    // TODO : FIX THIS
    if (this->shader_loaded_)
    {
        float cameraPos[3] = {this->camera_.position.x, this->camera_.position.y, this->camera_.position.z};
        SetShaderValue(this->shaders_["light"], this->shaders_["light"].locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        UpdateLightValues(this->shaders_["light"], this->light_);
    }

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
        bool disabled = disabled_groups[vis_object.group_id];
        if (!disabled)
        {
            this->render_visual_object(vis_object);
        }
    }

    // Draw The lines
    while (!this->lines_.empty())
    {
        Line line = this->lines_.front();
        DrawLine3D(line.start_pos, line.end_pos, line.color);
        this->lines_.pop();
    }
    // Draw The spheres
    while (!this->spheres_.empty())
    {
        VisSphere sphere = this->spheres_.front();
        DrawSphere(sphere.position, sphere.radius, sphere.color);
        this->spheres_.pop();
    }
    // Draw Arrows
    while (!this->arrows_.empty())
    {
        Arrow arrow = this->arrows_.front();
        du::draw_arrow(arrow.origin, Vector3Add(arrow.origin, arrow.vector), arrow.color, arrow.radius);
        this->arrows_.pop();
    }
    // Draw AABB
    while (!this->aabb_buffer_.empty())
    {
        AxisAlignedBoundingBox aabb = this->aabb_buffer_.front();
        DrawBoundingBox(aabb.bounding_box, aabb.color);
        this->aabb_buffer_.pop();
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
int Visualizer::add_box(Vector3 position, Quaternion orientation, Color color, float width, float height, float length, int group_id)
{
    Mesh cube_mesh = GenMeshCube(width, height, length);
    Model cube = LoadModelFromMesh(cube_mesh);
    VisualObject cube_vis_object = {
        .position = position,
        .orientation = orientation,
        .model = cube,
        .color = color,
        .group_id = group_id};

    return this->add_visual_object(cube_vis_object);
}

int Visualizer::add_sphere(Vector3 position, Quaternion orientation, Color color, float radius, int group_id)
{
    Mesh sphere_mesh = GenMeshSphere(radius, 16, 16);
    Model sphere = LoadModelFromMesh(sphere_mesh);
    VisualObject sphere_vis_object = {
        .position = position,
        .orientation = orientation,
        .model = sphere,
        .color = color,
        .group_id = group_id};

    return this->add_visual_object(sphere_vis_object);
}

int Visualizer::add_cylinder(Vector3 position, Quaternion orientation, Color color, float radius, float height, int group_id)
{
    Mesh cylinder_mesh = GenMeshCylinder(radius, height, 16);

    Model cylinder = LoadModelFromMesh(cylinder_mesh);
    cylinder.transform = MatrixMultiply(MatrixTranslate(0, -height * 0.5, 0), MatrixRotateX(PI / 2));
    VisualObject cylinder_vis_object = {
        .position = position,
        .orientation = orientation,
        .model = cylinder,
        .color = color,
        .group_id = group_id};

    return this->add_visual_object(cylinder_vis_object);
}

// int Visualizer::add_capsule(Vector3 position, Quaternion orientation, Color color, float radius, float height)
// {

//     // Set the position and orientation of the complete capsule model
//     //capsule_model.transform = du::get_transform(position, orientation);

//     // Create a VisualObject with the complete capsule model
//     VisualObject capsule_vis_object = {
//         position,
//         orientation,
//         capsule_model,
//         color
//     };

//     return this->add_visual_object(capsule_vis_object);
// }

int Visualizer::add_cone(Vector3 position, Quaternion orientation, Color color, float radius, float height, int group_id)
{
    Mesh cone_mesh = GenMeshCone(radius, height, 16);
    Model cone = LoadModelFromMesh(cone_mesh);
    VisualObject cone_vis_object = {
        .position = position,
        .orientation = orientation,
        .model = cone,
        .color = color,
        .group_id = group_id};

    return this->add_visual_object(cone_vis_object);
}

int Visualizer::add_plane(Vector3 position, Quaternion orientation, Color color, float width, float length, int group_id)
{
    Mesh plane_mesh = GenMeshPlane(width, length, 16, 16);
    Model plane = LoadModelFromMesh(plane_mesh);
    VisualObject plane_vis_object = {
        .position = position,
        .orientation = orientation,
        .model = plane,
        .color = color,
        .group_id = group_id};

    if (this->shader_loaded_)
    {
        this->shadow_texture = LoadRenderTexture(400, 400);
        plane_vis_object.model.materials[0].maps[RL_SHADER_LOC_MAP_ALBEDO].texture = this->shadow_texture.texture;
    }

    return this->add_visual_object(plane_vis_object);
}

int Visualizer::add_mesh(const char *filename, Vector3 position, Quaternion orientation, Color color, float scale, int group_id)
{
    Model model = LoadModel(filename);
    model.transform = MatrixScale(scale, scale, scale);
    VisualObject vis_object = {
        .position = position,
        .orientation = orientation,
        .model = model,
        .color = color,
        .group_id = group_id};

    return this->add_visual_object(vis_object);
}

int Visualizer::add_heightmap(Vector3 position,
                              Quaternion orientation,
                              Color color,
                              size_t width,
                              size_t height,
                              const std::vector<float> &data,
                              float x_scale,
                              float y_scale,
                              float z_scale,
                              int group_id)
{

    Image formated_data{
        .data = reinterpret_cast<void *>(const_cast<float *>(data.data())),
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PixelFormat::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};

    Vector3 size_vector = {x_scale, y_scale, z_scale};

    Mesh mesh = GenMeshHeightmap(formated_data, size_vector);

    int x_shift = width / 2;
    int y_shift = height / 2;

    Model model = LoadModelFromMesh(mesh);

    model.transform = MatrixMultiply(MatrixTranslate(-x_shift, 0, -y_shift), MatrixRotateX(PI / 2));

    VisualObject vis_object = {
        .position = position,
        .orientation = orientation,
        .model = model,
        .color = color,
        .group_id = group_id};

    return this->add_visual_object(vis_object);
};

void Visualizer::draw_line(Vector3 start_pos, Vector3 end_pos, Color color)
{
    Line line = {
        start_pos,
        end_pos,
        color};
    this->lines_.push(line);
}

void Visualizer::draw_sphere(Vector3 position, float radius, Color color)
{

    VisSphere sphere = {
        .position = position,
        .radius = radius,
        .color = color};
    this->spheres_.push(sphere);
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

void Visualizer::draw_aabb(Vector3 min, Vector3 max, Color color)
{
    BoundingBox bounding_box = {
        .min = min,
        .max = max};

    AxisAlignedBoundingBox aabb = {
        .bounding_box = bounding_box,
        .color = color};

    this->aabb_buffer_.push(aabb);
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
    this->light_ = CreateLight(LIGHT_DIRECTIONAL, (Vector3){0.0f, 100.0f, 0.0f}, Vector3Zero(), WHITE, this->shaders_["light"]);

    this->shaders_["light"].locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(this->shaders_["light"], "viewPos");

    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(this->shaders_["light"], "ambient");
    Vector4 ambient = {1.2f, 1.2f, 1.2f, .9f}; // Use Vector4 for ambient light
    SetShaderValue(this->shaders_["light"], ambientLoc, &ambient, SHADER_UNIFORM_VEC4);

    this->assing_lighting_to_models();
}

void Visualizer::assing_lighting_to_models()
{
    // Assign out lighting shader to model

    if (this->shader_loaded_)
    {

        for (VisualObject &vis_object : this->visual_objects_)
        {
            vis_object.model.materials[0].shader = this->shaders_["light"];
        }
    }
}

void Visualizer::load_shader(const char *filename_vs, const char *filename_fs, int group_id)
{
    // Shader shader = LoadShader(TextFormat(filename_vs, GLSL_VERSION), TextFormat(filename_fs, GLSL_VERSION));

    // SHADER_BASE_PATH is defined in the CMAKE file
    std::string vs_path = std::string(SHADER_BASE_PATH) + "/lighting.vs"; // + filename_vs;
    std::string fs_path = std::string(SHADER_BASE_PATH) + "/lighting.fs"; // + filename_fs;

    this->shaders_.insert({"light", LoadShader(TextFormat(vs_path.c_str(), GLSL_VERSION),
                                               TextFormat(fs_path.c_str(), GLSL_VERSION))});

    this->shaders_["light"].locs[RL_SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(this->shaders_["light"], "matModel");
    this->shaders_["light"].locs[RL_SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(this->shaders_["light"], "viewPos");

    this->shader_loaded_ = true;

    this->set_up_lighting();
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

void Visualizer::unload_models(void)
{
    // Unload all the models
    for (auto &vis_object : this->visual_objects_)
    {
        UnloadModel(vis_object.model);
    }

    this->visual_objects_ = {};
}

void Visualizer::set_imgui_interfaces(std::function<void(void)> func)
{
    this->imgui_interfaces_calls.push_back(func);
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

void Visualizer::render_visual_object(const VisualObject &vis_object)
{
    // Get the rotation axis and angle from the quaternion
    Vector3 axis;
    float angle;
    QuaternionToAxisAngle(vis_object.orientation, &axis, &angle);
    angle = angle * 180 / 3.1415926535;
    if (this->wireframe_mode_)
    {
        DrawModelWiresEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, vis_object.color);
    }
    else
    {
        DrawModelEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, vis_object.color);
        // Draw wireframe with a small offset in the color (make it darker)
        Color wireColor = {(unsigned char)(vis_object.color.r * 0.7f),
                           (unsigned char)(vis_object.color.g * 0.7f),
                           (unsigned char)(vis_object.color.b * 0.7f), 255};
        DrawModelWiresEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, wireColor);
    }

    if (this->show_bodies_coordinate_frame_)
    {
        du::draw_axes(vis_object.position, vis_object.orientation);
    }
}

void Visualizer::render_visual_object_shadow(const VisualObject &vis_object)
{
    // Get the rotation axis and angle from the quaternion
    Vector3 axis;
    float angle;
    QuaternionToAxisAngle(vis_object.orientation, &axis, &angle);
    angle = angle * 180 / 3.1415926535;
    if (this->wireframe_mode_)
    {
        DrawModelWiresEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, DARKGRAY);
    }
    else
    {
        DrawModelEx(vis_object.model, vis_object.position, axis, angle, {1.0f, 1.0f, 1.0f}, DARKGRAY);
    }
}

void Visualizer::disable_visual_object_group_rendering(int group_id)
{
    if (group_id < 255)
    {
        this->disabled_groups[group_id] = true;
    }
}

void Visualizer::enable_visual_object_group_rendering(int group_id)
{
    if (group_id < 255)
    {
        this->disabled_groups[group_id] = false;
    }
}
