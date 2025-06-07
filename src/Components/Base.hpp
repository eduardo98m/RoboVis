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
#include <map>
#include <functional>
#include <set>
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

    // Exclusive enums for the pointcloud type
    namespace PointCloudStyle
    {
        /// Marker rendering styles
        enum MarkerType
        {
            Cube,
            Square
        };

        /// Coloring strategies
        enum ColoringMode
        {
            SolidColor,
            ColorByAxisX,
            ColorByAxisY,
            ColorByAxisZ,
            PointColor // for RGB/rgba
        };
    }

    
    template <typename PointT>
    struct PointCloud
    {
        typename pcl::PointCloud<PointT>::Ptr cloud = std::make_shared<pcl::PointCloud<PointT>>();

        Vector3 position{};
        Quaternion orientation{};
        Color color{};  // Used only if coloring_mode == SolidColor

        PointCloudStyle::MarkerType marker_type = PointCloudStyle::MarkerType::Cube;
        PointCloudStyle::ColoringMode coloring_mode = PointCloudStyle::ColoringMode::SolidColor;

        bool visible = true;
        float scale = 1.0f;
        int ssboID = 0;
        int color_ssboID= 0;

        // Position and color buffers (For passing data to the shader / Only used on the RGBA pointclouds)
        std::vector<Vector4> pos_buffer = {};
        std::vector<Vector4> color_buffer = {};
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

    struct ChunkInfo{
        Vector3 position;    // Position of the chunk relative to the map coordinates
        std::string model_path; // path of the model to be loaded
    };

    struct ChunkKey {
        size_t chunk_x_id;
        size_t chunk_y_id;
        size_t chunk_z_id;
        size_t lod;
    
        bool operator==(const ChunkKey &other) const {
            return chunk_x_id == other.chunk_x_id && 
                   chunk_y_id == other.chunk_y_id && 
                   chunk_z_id == other.chunk_z_id && 
                   lod == other.lod;
        }
        // This operator allow us to use the chunk key as a key for a std::map
        // To use an urordered map we would need to implement a hash function
        bool operator<(const ChunkKey& other) const {
            return std::tie(chunk_x_id, chunk_y_id, chunk_z_id, lod) <
                   std::tie(other.chunk_x_id, other.chunk_y_id, other.chunk_z_id, other.lod);
        }
    };

    struct Map {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        size_t lods;
        Vector3 chunk_size;
        std::map<ChunkKey, ChunkInfo> chunks;
        std::map<ChunkKey, std::unique_ptr<Model>> loaded_chunks;
        std::set<ChunkKey> visible_chunks;
        bool visible = true;
    };
}
