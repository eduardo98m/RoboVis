#pragma once
#include "Systems/Serialization.hpp"

namespace rbvs
{
    namespace fs = std::filesystem;

    // Helper for PointCloudStyle::MarkerType to string (example, implement fully

    template <typename PointT>
    std::string SerializationSystem::parse_pointcloud(
        const PointCloud<PointT> &pc,
        const std::string &entity_id_str,
        const std::string &pcl_storage_abs_path,
        const std::string &pcl_xml_rel_path_prefix)
    {
        std::ostringstream oss;
        oss << "  <pointcloud";
        oss << " name=\"" << "pcl_" << entity_id_str << "\"";
        oss << " point_type=\"" << typeid(PointT).name() << "\""; // Or a more user-friendly type name
        oss << " visible=\"" << (pc.visible ? "true" : "false") << "\"";
        oss << " position=\"" << format_vec3_space_separated(pc.position) << "\"";
        oss << " orientation_rpy=\"" << format_quat_to_euler_rpy_space_separated(pc.orientation) << "\"";
        oss << " scale_val=\"" << std::fixed << std::setprecision(6) << pc.scale << "\""; // pc.scale is float
        oss << " color=\"" << format_color_space_separated(pc.color) << "\"";             // If solid color mode
        oss << " marker_type=\"" << marker_type_to_string(pc.marker_type) << "\"";
        oss << " coloring_mode=\"" << coloring_mode_to_string(pc.coloring_mode) << "\"";

        if (pc.cloud && !pc.cloud->points.empty())
        {
            std::string pcd_filename_stem = entity_id_str;
            std::string pcd_filename_with_ext = pcd_filename_stem + ".pcd";
            fs::path pcd_abs_filepath = fs::path(pcl_storage_abs_path) / pcd_filename_with_ext;
            fs::path pcd_rel_filepath_for_xml = fs::path(pcl_xml_rel_path_prefix) / pcd_filename_with_ext;

            if (pcl::io::savePCDFileASCII(pcd_abs_filepath.string(), *pc.cloud) == 0)
            {
                oss << " pcd_file=\"" << sanitize_path(pcd_rel_filepath_for_xml.string()) << "\"";
            }
            else
            {
                std::cerr << "Warning: Failed to save PCD file for entity " << entity_id_str << " to " << pcd_abs_filepath.string() << std::endl;
                oss << " pcd_file=\"SAVE_ERROR\"";
            }
        }
        else
        {
            oss << " pcd_file_note=\"Cloud is null or empty\"";
        }
        oss << " />" << std::endl;
        return oss.str();
    }

}
