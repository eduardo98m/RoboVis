/**
 * @brief 
 */
#pragma once
// raylib
#include <raylib.h>
// std
#include <iostream>
#include <memory>
#include <vector>
#include <optional>
// pcl
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

// Basic componentes
namespace rbvs
{
    struct VisualModel
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        std::unique_ptr<Model> model;
        bool visible = true;
    };


    struct DebugShape
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        float lifetime;
    };

    struct TextLabel
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        float lifetime;
    };

    struct PointCloud
    {
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;
        Vector3 position;
        Quaternion orientation;
        Color color;

        enum MarkerType {Cube, Square} marker_type = Cube;  // New option for coloring mode
        enum ColoringMode { SolidColor, ColorByAxisX, ColorByAxisY, ColorByAxisZ } coloring_mode = ColoringMode::SolidColor;  // New option for coloring mode
        
        bool visible = true;
        float scale = 1.0;
        int ssboID = 0;
        
    };
    
    struct Gizmo
    {
        Transform transform;
        bool visible = true;
    };


    struct HeightMap
    {
        Vector3 position; // Position of the center of the heightma
        Quaternion orientation; // Orientation of the heightmap
        Vector3 scale; // Scale of the heightmap (x, y) are the side_lenghs z is for the heights scale
        std::unique_ptr<Mesh> mesh;
        Color color; // Uniform color if  color map is enabled (If the color_map is given this will apply a tint)
        std::optional<Texture> color_map; //  paramter for the heightmap color (Useful for cost-maps) (enabled if no nullptr)
        bool visible = true;
    };
        
}
