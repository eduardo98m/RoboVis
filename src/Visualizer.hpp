
#pragma once

#include<raylib.h>
#include<raymath.h>
#include<vector>

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
        std::vector<VisualObject> visual_objects_;

    public:
        Visualizer(int screen_width, int screen_height, const char* title);
        ~Visualizer();
        void update();
        void close();
        void set_up_camera();
        uint add_visual_object(VisualObject vis_object);
        void update_visual_object_position_orientation(uint index, Vector3 position, Quaternion orientation);
        void remove_visual_object(uint index);
        void clear_visual_objects();
        void draw_shader();
};
