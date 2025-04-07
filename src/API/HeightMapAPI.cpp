#include "VisualizerNew.hpp"

namespace rbvs
{   

    Texture create_heightmap_texture(const std::vector<float> &heights, int n_x, int n_y){
        // Create an Image structure from the pixel buffer.
        Image formated_heightmap{
            .data = reinterpret_cast<void *>(const_cast<float *>(heights.data())),
            .width = n_x,
            .height = n_y,
            .mipmaps = 1,
            .format = PixelFormat::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};

        // Load a texture from the Image.
        Texture2D tex = LoadTextureFromImage(formated_heightmap);
        // Optionally, set texture filter modes.
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

        return tex;
    };

    Entity Visualizer::create_height_map(HeightMapParams params)
    {

        // We only upload the texture once to the GPU
        Texture heightmap = create_heightmap_texture(params.heights, params.n_x, params.n_y);

        HeightMap hm = {
             .position = params.position, 
             .orientation = params.orientation,               
             .scale = params.scale, 
             .heights = heightmap, 
             .color = params.color, 
             //.color_map = std::move(params.color_map), 
        };

        Entity e = this->entity_manager.create();
        this->entity_manager.addComponent<HeightMap>(e, std::move(hm));

        return e;
    }


    void Visualizer::update_height_map(HeightMapUpdateParams params)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doenst have the component)
        HeightMap &hm = this->entity_manager.getComponent<HeightMap>(params.entity);
        
        if (params.position) hm.position = *params.position;
        if (params.orientation) hm.orientation = *params.orientation;
        if (params.scale) hm.scale = *params.scale;

        // We might need to unload the texture from the GPU
        if (params.heights && params.n_x && params.n_y) hm.heights = create_heightmap_texture(**params.heights, *params.n_x, *params.n_y);

        //if (params.heights) hm.heights =  std::move(*params.heights);
        // if (params.n_x) hm.n_x =  *params.n_x;
        // if (params.n_y) hm.n_y =  *params.n_y;
        //if (params.color_map) hm.color_map = std::move(*params.color_map);
        if (params.color) hm.color = *params.color; 
        if (params.visible) hm.visible = *params.visible;      
    }


}
