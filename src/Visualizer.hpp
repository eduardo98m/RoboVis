
#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <queue>
#include "rlImGui.h"
#include "imgui.h"
#include "raymath.h"
#include "rlights.hpp"
#include <iostream>
#include <functional>

#define GLSL_VERSION 330

/**
 * @brief Represents a 3D visual object with position, orientation, model, color, and group ID.
 */
struct VisualObject
{
    Vector3 position;       // Position of the visual object.
    Quaternion orientation; // Orientation of the visual object.
    Model model;            // Model associated with the visual object.
    Color color;            // Color of the visual object.
    int group_id = 0;       // Group ID to which the visual object belongs.
};

struct Line
{
    Vector3 start_pos; // Start position of the line
    Vector3 end_pos;   // End position of the line
    Color color;       // Color of the line
};

struct Point
{
    Vector3 position;
    Color color;
};

struct TextLabel{
    const char *text;
    Vector3 position;
    float fontSize = 500.0;
    Color color = WHITE;
    Font font = GetFontDefault();
    bool background;
    Color backgroundColor = BLACK;  
    bool enabled = true;
};

/**
 * @brief Manages the visualization of 3D objects, camera, lighting, and shaders.
 */
class Visualizer
{
private:
    const int screen_width_;                            // Screen width for visualization.
    const int screen_height_;                           // Screen height for visualization.
    const char *title_;                                 // Title of the visualization window.
    Camera camera_;      // Camera for viewing the scene.
    Shader shader_;                                     // Shader used for rendering.
    bool shader_loaded_ = false;                        // Flag indicating whether the shader is loaded.
    std::vector<VisualObject> visual_objects_;          // List of visual objects in the scene.
    std::vector<Point> points_;
    std::queue<Line> lines_;
    std::vector<TextLabel> text_labels_;
    bool wireframe_mode_;      // Flag indicating whether to render in wireframe mode.
    int focused_object_index_; // Index of the focused visual object.
    int previously_focused_object_index_ = -2;
    bool focus_mode_ = false;
    RenderTexture2D shader_target_; // Render target for shaders.
    Light light_;                   // Lighting setup for the scene.
    bool show_bodies_coordinate_frame_ = false;

    std::function<void(void)>  imgui_interfaces_calls = [](void) -> void {return;};

public:
    /**
     * @brief Constructor for the Visualizer class.
     * @param screen_width The width of the visualization window.
     * @param screen_height The height of the visualization window.
     * @param title Title of the visualization window.
     */
    Visualizer(int screen_width, int screen_height, const char *title);

    /**
     * @brief Destructor for the Visualizer class.
     */
    ~Visualizer();

    /**
     * @brief Updates the visualizer, including camera and shader updates.
     */
    void update();

    /**
     * @brief Draws the graphical user interface (GUI) for the visualizer.
     */
    void draw_gui();

    /**
     * @brief Draws the shader effects on the scene.
     */
    void draw_shader();

    /**
     * @brief Closes the visualization window.
     */
    void close();

    // Camera
    /**
     * @brief Sets up the camera with default parameters.
     */
    void set_up_camera();

    /**
     * @brief Updates the camera based on the keyboard and mouse inputs.
     */
    void update_camera();

    /**
     * @brief Sets the camera focus based on the focused object.
     */
    void set_camera_focus();

    // Lighting
    /**
     * @brief Sets up the lighting for the scene.
     */
    void set_up_lighting();

    // Shader
    /**
     * @brief Loads and sets the shader for rendering.
     * @param filename_fs Path to the fragment shader file.
     * @param filename_vs Path to the vertex shader file.
     * @param group_id Group ID associated with the shader.
     */
    void load_shader(const char *filename_fs, const char *filename_vs, int group_id);

    //--------------------//

    /**
     * @brief Adds a visual object to the scene.
     * @param vis_object The visual object to be added.
     * @return The index of the added visual object.
     */
    int add_visual_object(VisualObject vis_object);

    /**
     * @brief Updates the position, orientation, and scale of a visual object.
     * @param index Index of the visual object to be updated.
     * @param position New position of the visual object.
     * @param orientation New orientation of the visual object.
     * @param scale New scale of the visual object.
     */
    void update_visual_object_position_orientation_scale(int index, Vector3 position, Quaternion orientation, Vector3 scale);

    /**
     * @brief Updates the position and orientation of a visual object.
     * @param index Index of the visual object to be updated.
     * @param position New position of the visual object.
     * @param orientation New orientation of the visual object.
     */
    void update_visual_object_position_orientation(int index, Vector3 position, Quaternion orientation);

    /**
     * @brief Updates the scale of a visual object.
     * @param index Index of the visual object to be updated.
     * @param scale New scale of the visual object.
     */
    void update_visual_object_scale(int index, Vector3 scale);

    /**
     * @brief Removes a visual object from the scene.
     * @param index Index of the visual object to be removed.
     */
    void remove_visual_object(int index);

    /**
     * @brief Clears all visual objects from the scene.
     */
    void clear_visual_objects();

    // Functions to draw geometric primitives

    /**
     * @brief Adds a cube to the scene.
     * @param position Position of the cube.
     * @param orientation Orientation of the cube.
     * @param color Color of the cube.
     * @param width Width of the cube.
     * @param height Height of the cube.
     * @param length Length of the cube.
     * @return The index of the added cube.
     */
    int add_box(Vector3 position, Quaternion orientation, Color color, float width, float height, float length);

    /**
     * @brief Adds a sphere to the scene.
     * @param position Position of the sphere.
     * @param orientation Orientation of the sphere.
     * @param color Color of the sphere.
     * @param radius Radius of the sphere.
     * @return The index of the added sphere.
     */
    int add_sphere(Vector3 position, Quaternion orientation, Color color, float radius);

    /**
     * @brief Adds a cylinder to the scene.
     * @param position Position of the cylinder.
     * @param orientation Orientation of the cylinder.
     * @param color Color of the cylinder.
     * @param radius Radius of the cylinder.
     * @param height Height of the cylinder.
     * @return The index of the added cylinder.
     */
    int add_cylinder(Vector3 position, Quaternion orientation, Color color, float radius, float height);

    /**
     * @brief Adds a cone to the scene.
     * @param position Position of the cone.
     * @param orientation Orientation of the cone.
     * @param color Color of the cone.
     * @param radius Radius of the cone.
     * @param height Height of the cone.
     * @return The index of the added cone.
     */
    int add_cone(Vector3 position, Quaternion orientation, Color color, float radius, float height);

    /**
     * @brief Adds a plane to the scene.
     * @param position Position of the plane.
     * @param orientation Orientation of the plane.
     * @param color Color of the plane.
     * @param width Width of the plane.
     * @param length Length of the plane.
     * @return The index of the added plane.
     */
    int add_plane(Vector3 position, Quaternion orientation, Color color, float width, float length);

    // Uncomment the following line when the heightmap function is implemented.
    // int add_heightmap(Vector3 position, Quaternion orientation, Color color, std::vector<std::vector<float>> heightmap);

    /**
     * @brief Adds a mesh to the scene.
     * @param filename Path to the mesh file.
     * @param position Position of the mesh.
     * @param orientation Orientation of the mesh.
     * @param color Color of the mesh.
     * @param scale Scale of the mesh.
     * @return The index of the added mesh.
     */
    int add_mesh(const char *filename, Vector3 position, Quaternion orientation, Color color, float scale);

    // Helper drawing functions:
    // This set of functions purposes are to let the user draw visual indicators:
    // * Lines, Text

    /**
     *
     */

    void draw_line(Vector3 start_pos, Vector3 end_pos, Color color = WHITE);

    // int add_point()

    void draw_text_label(TextLabel label);

    int add_text_label(const char *text, Vector3 position, float font_size = 500.0, bool background = true, Color color  = WHITE, Font font = GetFontDefault(), Color background_color = BLACK);

    void modify_text_label(int index, const char *text);

    void modify_text_label(int index, std::string text);

    void modify_text_position(int index, Vector3 position);

    void set_imgui_interfaces(std::function<void(void)> func);
};
