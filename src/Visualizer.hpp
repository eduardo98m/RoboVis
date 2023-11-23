
#pragma once
#include<raylib.h>
#include<raymath.h>
#include<vector>
#include "rlImGui.h"	
#include "imgui.h"
#include<iostream>

#define GLSL_VERSION            330

struct VisualObject {
    Vector3 position;
    Quaternion orientation;
    Model model;
    Color color; 
};

class Visualizer {
    private:
        const int screen_width_;
        const int screen_height_;
        const char* title_;
        Camera camera_;
        Shader shader_;
        bool shader_loaded_ = false;
        std::vector<VisualObject> visual_objects_;
        bool wireframe_mode_;
        int focused_object_index_;
        RenderTexture2D shader_target_;

    public:
        Visualizer(int screen_width, int screen_height, const char* title);
        ~Visualizer();
        void update();
        void draw_gui();
        void draw_shader();
        void close();
        // Camera
        void set_up_camera();
        void set_camera_focus();
        // Shader
        // filename -> path to the shader file (.fs)
        void load_shader(const char* filename);
        //--------------------//
        uint add_visual_object(VisualObject vis_object);
        void update_visual_object_position_orientation(uint index, Vector3 position, Quaternion orientation);
        void remove_visual_object(uint index);
        void clear_visual_objects();
        // Functions to draw geometric primitives
        uint add_cube(Vector3 position, Quaternion orientation, Color color, float width, float height, float length);
        uint add_sphere(Vector3 position, Quaternion orientation, Color color, float radius);
        uint add_cylinder(Vector3 position, Quaternion orientation, Color color, float radius, float height);
        uint add_cone(Vector3 position, Quaternion orientation, Color color, float radius, float height);
        uint add_plane(Vector3 position, Quaternion orientation, Color color, float width, float length);
        //uint add_heightmap(Vector3 position, Quaternion orientation, Color color, std::vector<std::vector<float>> heightmap);
        uint add_mesh(const char *filename, Vector3 position, Quaternion orientation, Color color, float scale);


};
