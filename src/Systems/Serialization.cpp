#include "Systems/Serialization.hpp"

namespace rbvs
{
    namespace fs = std::filesystem;

    // Helper to create directories (basic version)
    // std::filesystem::create_directories is more robust
    inline bool create_directories_recursive(const std::string &full_path_str)
    {
        namespace fs = std::filesystem;
        fs::path dir_path(full_path_str);
        try
        {
            return fs::create_directories(dir_path);
        }
        catch (const fs::filesystem_error &e)
        {
            std::cerr << "Error creating directory " << full_path_str << ": " << e.what() << std::endl;
            return false;
        }
    }

    SerializationSystem::SerializationSystem()
    {
    }

    void SerializationSystem::serialize(EntityManager &em, std::string path, std::string name)
    {
        std::string basePathStr = sanitize_path(path);
        std::string worldNameStr = sanitize_path(name);

        fs::path rootDir = fs::path(basePathStr) / worldNameStr;
        fs::path pclDir = rootDir / "pcl";
        fs::path meshesDir = rootDir / "meshes";
        fs::path heightmapsDir = rootDir / "heightmaps"; // Also for heightmap textures

        std::cout << "SerializationSystem: Starting serialization..." << std::endl;
        std::cout << "Base path: " << basePathStr << std::endl;
        std::cout << "World name: " << worldNameStr << std::endl;
        std::cout << "Root directory: " << rootDir.string() << std::endl;

        if (!create_directories_recursive(rootDir.string()))
        {
            std::cerr << "Failed to create root directory: " << rootDir.string() << std::endl;
            return;
        }
        if (!create_directories_recursive(pclDir.string()))
        {
            std::cerr << "Failed to create pcl directory: " << pclDir.string() << std::endl;
            return;
        }
        if (!create_directories_recursive(meshesDir.string()))
        {
            std::cerr << "Failed to create meshes directory: " << meshesDir.string() << std::endl;
            return;
        }
        if (!create_directories_recursive(heightmapsDir.string()))
        {
            std::cerr << "Failed to create heightmaps directory: " << heightmapsDir.string() << std::endl;
            return;
        }

        fs::path xmlFilePath = rootDir / "scene.xml";
        std::ofstream xmlFile(xmlFilePath.string());

        if (!xmlFile.is_open())
        {
            std::cerr << "Failed to open XML file for writing: " << xmlFilePath.string() << std::endl;
            return;
        }

        std::cout << "Output XML file: " << xmlFilePath.string() << std::endl;

        xmlFile << "<world>" << std::endl;

        // For unique file naming if entity IDs are not directly usable as strings
        int fileCounter = 0;

        // Serialize VisualModels
        // auto view_vm = em.view<VisualModel>();
        // for (const auto entity : view_vm) {
        //     auto &vm = em.getComponent<VisualModel>(entity);
        //     // entt::to_integral(entity) might give a usable ID.
        //     // For simplicity, we'll use a counter for filenames here,
        //     // but using entity ID is better if possible.
        //     std::string entity_id_str = "entity_vm_" + std::to_string(entity.id); // Example ID
        //     xmlFile << parse_visual_model(vm, entity_id_str, meshesDir.string(), "meshes");
        //     fileCounter++;
        // }

        // Serialize PointClouds (Example for PointXYZRGBA, adapt for other types)
        // You might need to register specific point cloud types you use.
        auto view_pc_xyz_rgba = em.view<PointCloud<pcl::PointXYZRGBA>>();
        for (const auto entity : view_pc_xyz_rgba)
        {
            auto &pc = em.getComponent<PointCloud<pcl::PointXYZRGBA>>(entity);
            std::string entity_id_str = "entity_pc_rgba_" + std::to_string(entity.id);
            xmlFile << parse_pointcloud(pc, entity_id_str, pclDir.string(), "pcl");
            fileCounter++;
        }

        auto view_pc_xyz = em.view<PointCloud<pcl::PointXYZ>>();
        for (const auto entity : view_pc_xyz)
        {
            auto &pc = em.getComponent<PointCloud<pcl::PointXYZ>>(entity);
            std::string entity_id_str = "entity_pc_rgba_" + std::to_string(entity.id);
            xmlFile << parse_pointcloud(pc, entity_id_str, pclDir.string(), "pcl");
            fileCounter++;
        }

        xmlFile << "</world>" << std::endl;
        xmlFile.close();
        std::cout << "SerializationSystem: Serialization completed." << std::endl;
    }

    // Helper to convert Vector3 to XML string
    // Helper functions to convert attributes
    std::string SerializationSystem::format_vec3_space_separated(const Vector3 &v)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6); // Consistent precision
        oss << v.x << " " << v.y << " " << v.z;
        return oss.str();
    }

    // Output format: "roll pitch yaw" (space separated, values in radians)
    // Raylib QuaternionToEuler returns Vector3(pitch, yaw, roll)
    std::string SerializationSystem::format_quat_to_euler_rpy_space_separated(const Quaternion &q)
    {
        Vector3 eulerAnglesRad = QuaternionToEuler(q); // X: pitch, Y: yaw, Z: roll
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6);
        // Outputting as Roll, Pitch, Yaw
        oss << eulerAnglesRad.z << " " << eulerAnglesRad.x << " " << eulerAnglesRad.y;
        return oss.str();
    }

    // Output format: "r g b a" (space separated)
    std::string SerializationSystem::format_color_space_separated(const Color &c)
    {
        std::ostringstream oss;
        oss << static_cast<int>(c.r) << " "
            << static_cast<int>(c.g) << " "
            << static_cast<int>(c.b) << " "
            << static_cast<int>(c.a);
        return oss.str();
    }

}
