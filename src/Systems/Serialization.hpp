/**
 * @brief In this file is describe the serialziation system this is a system that takes as input the entity manager and
 * goest trough all of the relevant components (Pointcluds, Heightmaps, VisualModels) and saves the attributes of these models
 * in an xml format to later load the "world" (deserialize)
 *
 * Definition of the format
 * It opens with the <world>
 */
#pragma once

#include "Components/Base.hpp"
#include "ECS/EntityManager.hpp"

// STD
#include <string>
#include <fstream>    // For std::ofstream
#include <sstream>    // For std::ostringstream
#include <iomanip>    // For std::fixed, std::setprecision
#include <algorithm>  // For std::replace for paths if needed
#include <filesystem> // Recommended for C++17+ for path handling

// Raylib
#include <raymath.h>

// PCL
#include <pcl/io/ply_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/auto_io.h>

namespace rbvs
{

    inline std::string sanitize_path(std::string p)
    {
        std::replace(p.begin(), p.end(), '\\', '/');
        return p;
    }

    class SerializationSystem
    {
    public:
        SerializationSystem();

        /**
         * @brief Saves the current state if the visualization into an xml format
         * @param em The entity manager
         * @param path The path (absolute/relative) where the world is going to be saved
         * @param name The name of the directory that is going to b e created and where the files will be saved
         *
         * The function must creaet a directionry in `path` with the name `name` (i.e. path/name)
         * It should create the folders `pcl` `meshes` `heightmaps` in the path/name folder
         *
         */
        void serialize(EntityManager &em, std::string path, std::string name);

    private:
        /**
         * For the models we can save into the xml the simpler ones like cubes, sphere, cylinders and cones
         * For meshes we need to save the mesh file and
         */
        std::string parse_visual_model(const VisualModel &vm,
                                       const std::string &entity_id,
                                       const std::string &meshes_abs_path,
                                       const std::string &meshes_rel_path);

        /**
         * @brief Parses a heightmap into its xml tag
         */
        std::string parse_heightmap(HeightMap hm);
        /**
         * For the pointclouds is harder because we cannot save the pointcloud data into .pcd format
         */
        template <typename PointT>
        std::string parse_pointcloud(
            const PointCloud<PointT> &pc,
            const std::string &entity_id_str,
            const std::string &pcl_storage_abs_path,
            const std::string &pcl_xml_rel_path_prefix);

        // Helper functions to convert attributes
        std::string format_vec3_space_separated(const Vector3 &v);

        // Output format: "roll pitch yaw" (space separated, values in radians)
        // Raylib QuaternionToEuler returns Vector3(pitch, yaw, roll)
        std::string format_quat_to_euler_rpy_space_separated(const Quaternion &q);

        // Output format: "r g b a" (space separated)
        std::string format_color_space_separated(const Color &c);

        std::string marker_type_to_string(PointCloudStyle::MarkerType mt)
        {
            switch (mt)
            {
            case PointCloudStyle::MarkerType::Cube:
                return "Cube";
            case PointCloudStyle::MarkerType::Square:
                return "Square";
            default:
                return "Unknown";
            }
        }

        std::string coloring_mode_to_string(PointCloudStyle::ColoringMode cm)
        {
            switch (cm)
            {
            case PointCloudStyle::ColoringMode::SolidColor:
                return "SolidColor";
            case PointCloudStyle::ColoringMode::ColorByAxisX:
                return "ColorByAxisX";
            case PointCloudStyle::ColoringMode::ColorByAxisY:
                return "ColorByAxisY";
            case PointCloudStyle::ColoringMode::ColorByAxisZ:
                return "ColorByAxisZ";
            case PointCloudStyle::ColoringMode::PointColor:
                return "PointColor";
            default:
                return "Unknown";
            }
        }
    };
};

// Include the template
#include "Systems/Serialization.tpp"