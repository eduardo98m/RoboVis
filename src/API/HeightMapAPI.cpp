#include "VisualizerNew.hpp"

namespace rbvs
{

    Texture create_heightmap_color_texture(const std::vector<Color> &color_map, int n_x, int n_y)
    {
        // Create an Image structure from the pixel buffer.
        Image formated_heightmap{
            .data = reinterpret_cast<void *>(const_cast<Color *>(color_map.data())),
            .width = n_x,
            .height = n_y,
            .mipmaps = 1,
            .format = PixelFormat::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
        
        
        // Load a texture from the Image.
        Texture2D tex = LoadTextureFromImage(formated_heightmap);
        // Optionally, set texture filter modes.
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

        return tex;
    };

    Mesh create_heightmap_mesh(const std::vector<float> &heights, int n_x, int n_y)
    {
        Image formated_heightmap{
            .data = reinterpret_cast<void *>(const_cast<float *>(heights.data())),
            .width = n_x,
            .height = n_y,
            .mipmaps = 1,
            .format = PixelFormat::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};
        
        return GenMeshHeightmap(formated_heightmap, (Vector3){1.0, 1.0, 1.0});
    }

    Entity Visualizer::create_height_map(const HeightMapParams &params)
    {

        // We only upload the texture once to the GPU

        HeightMap hm = {
            .position = params.position,
            .orientation = params.orientation,
            .scale = params.scale,
            .mesh = std::make_unique<Mesh>(create_heightmap_mesh(params.heights, params.n_x, params.n_y)),
            .color = params.color,
        };

        if  (params.color_map){
            hm.color_map =  create_heightmap_color_texture(*params.color_map, params.n_x, params.n_y);
        }
        
        Entity e = this->entity_manager.create();
        this->entity_manager.addComponent<HeightMap>(e, std::move(hm));

        return e;
    }

    void Visualizer::update_height_map(const HeightMapUpdateParams &params)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doenst have the component)
        HeightMap &hm = this->entity_manager.getComponent<HeightMap>(params.entity);

        if (params.position)
            hm.position = *params.position;
        if (params.orientation)
            hm.orientation = *params.orientation;
        if (params.scale)
            hm.scale = *params.scale;

        // We might need to unload the texture from the GPU
        if (params.heights && params.n_x && params.n_y){
            if (hm.mesh) {
                UnloadMesh(*hm.mesh); // Free GPU memory - (Without this line we will have a memory leak)
            }
            hm.mesh = std::make_unique<Mesh>(create_heightmap_mesh(*params.heights, *params.n_x, *params.n_y));
        }

        if (params.color_map && params.n_x && params.n_y){
            if(hm.color_map) UnloadTexture(*hm.color_map); // Only unload the texture if it exists  

            hm.color_map = create_heightmap_color_texture(*params.color_map, *params.n_x, *params.n_y);
        }
            
        if (params.color)
            hm.color = *params.color;
        if (params.visible)
            hm.visible = *params.visible;
    }
}
