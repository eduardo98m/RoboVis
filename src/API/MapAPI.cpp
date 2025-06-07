#include "VisualizerNew.hpp"

namespace rbvs
{
    Entity Visualizer::create_map(MapParams params)
    {
        Entity e = this->entity_manager.create();

        Map map;
        map.position = params.position;
        map.orientation = params.orientation;
        map.scale = params.scale;

        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(params.map_data_path.c_str()) != tinyxml2::XML_SUCCESS)
        {
            std::cerr << "Failed to load metadata XML: " << params.map_data_path << std::endl;
            return e;
        }

        tinyxml2::XMLElement *root = doc.FirstChildElement("MapMetadata");
        if (!root)
            return e;

        tinyxml2::XMLElement *chunkSize = root->FirstChildElement("ChunkSize");
        tinyxml2::XMLElement *lods = root->FirstChildElement("LODs");
        tinyxml2::XMLElement *model = root->FirstChildElement("Model");
        Vector3 blender_chunk_size;

        if (chunkSize)
        {
            // Swizzle Z and Y due to coordinate system change
            blender_chunk_size = {
                chunkSize->FloatAttribute("x") * map.scale.x,
                chunkSize->FloatAttribute("y") * map.scale.y,
                chunkSize->FloatAttribute("z") * map.scale.z
            };

            map.chunk_size = {
                blender_chunk_size.x,
                blender_chunk_size.z,
                blender_chunk_size.y
            };
        }

        if (lods)
        {
            map.lods = lods->UnsignedAttribute("count");
        }

        std::string format = "obj";
        std::string base_path = fs::path(params.map_data_path).parent_path().string();
        std::regex chunk_name_pattern(R"(TerrainChunk_(\d+)_(\d+)_(\d+)(?:\.\d+)?\.obj)");
        std::string model_name = model->Attribute("name");

        for (size_t lod = 0; lod < map.lods; ++lod)
        {
            std::string folder_name = model_name + "_LOD" + std::to_string(lod) + "_Chunks";
            fs::path folder_path = fs::path(base_path) / folder_name;

            if (!fs::exists(folder_path))
                continue;

            // Track bounding box of chunk IDs to center the map
            int min_x = INT_MAX, min_y = INT_MAX, min_z = INT_MAX;
            int max_x = INT_MIN, max_y = INT_MIN, max_z = INT_MIN;

            std::vector<std::pair<ChunkKey, ChunkInfo>> chunk_list;

            for (const auto &entry : fs::directory_iterator(folder_path))
            {
                if (entry.path().extension() != ".obj")
                    continue;

                std::smatch match;
                std::string filename = entry.path().filename().string();

                if (!std::regex_match(filename, match, chunk_name_pattern))
                    continue;

                ChunkKey key;
                key.chunk_x_id = std::stoi(match[1]);
                key.chunk_y_id = std::stoi(match[3]);
                key.chunk_z_id = std::stoi(match[2]);
                key.lod = lod;

                ChunkInfo info;
                info.model_path = entry.path().string();

                // Track min/max for centering
                min_x = std::min(min_x, int(key.chunk_x_id));
                min_y = std::min(min_y, int(key.chunk_y_id));
                min_z = std::min(min_z, int(key.chunk_z_id));
                max_x = std::max(max_x, int(key.chunk_x_id));
                max_y = std::max(max_y, int(key.chunk_y_id));
                max_z = std::max(max_z, int(key.chunk_z_id));

                chunk_list.emplace_back(key, info);
            }

            // Compute center offset for centering chunks around origin
            float offset_x = ((min_x + max_x + 1) * 0.5f) * blender_chunk_size.x;
            float offset_y = ((min_y + max_y + 1) * 0.5f) * blender_chunk_size.y;
            float offset_z = ((min_z + max_z + 1) * 0.5f) * blender_chunk_size.z;

            for (auto &[key, info] : chunk_list)
            {
                // Blender uses (X, Y, Z), raylib expects (X, Z, -Y)
                float blender_x = (key.chunk_x_id + 0.5f) * blender_chunk_size.x;
                float blender_y = (key.chunk_y_id + 0.5f) * blender_chunk_size.y;
                float blender_z = (key.chunk_z_id + 0.5f) * blender_chunk_size.z;

                // Convert to raylib Y-up and apply centering
                info.position = {
                    blender_x - offset_x,
                    blender_z + offset_z * 0.25f,
                    blender_y - offset_y// flip Y and offset
                };

                map.chunks.insert({key, info});


                // Preload model
                // ChunkMetaData meta;
                // meta.model = 
                // meta.state = MapChunkState::HIDDEN;
                // meta.last_active_time = 0.0;
                map.loaded_chunks[key] = std::make_unique<Model>(LoadModel(info.model_path.c_str()));
            }
        }

        this->entity_manager.addComponent<Map>(e, std::move(map));
        return e;
    }

    void Visualizer::update_map(MapUpdateParams params)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doenst have the component)
        if (!this->entity_manager.hasComponent<Map>(params.entity))
            return;
        Map &map = this->entity_manager.getComponent<Map>(params.entity);
        if (params.position)
            map.position = *params.position;
        if (params.orientation)
            map.orientation = *params.orientation;
        if (params.scale)
            map.scale = *params.scale;
        if (params.visible)
            map.visible = *params.visible;
    }

    void Visualizer::delete_map(Entity entity)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doenst have the component)
        this->entity_manager.destroy(entity);
    }

}
