/**
 * This file includes all that is related to adding visual objects to the visualizer
 * Visual objects are stored as shared pointers in a vector, these are different from 
 * drawn objects because visual objects are persistent.
*/
#include "Visualizer.hpp"



int Visualizer::add_visual_object(std::shared_ptr<VisualObject> vis_object)
{
    // Add the shader to the visual object (if the shader is loaded)
    
    
    if (this->shader_loaded_)
    {
        vis_object->model.materials[0].shader = this->shaders_["light"];
    }
    this->visual_objects_.push_back(vis_object);

    return this->visual_objects_.size() - 1;
}

void Visualizer::update_visual_object_position_orientation(int index, Vector3 position, Quaternion orientation)
{
    this->visual_objects_[index]->position = position;
    this->visual_objects_[index]->orientation = orientation;
}

void Visualizer::update_visual_object_position_orientation_scale(int index, Vector3 position, Quaternion orientation, Vector3 scale)
{
    this->visual_objects_[index]->position = position;
    this->visual_objects_[index]->orientation = orientation;
    this->visual_objects_[index]->model.transform = MatrixScale(scale.x, scale.y, scale.z);
}

void Visualizer::update_visual_object_scale(int index, Vector3 scale)
{
    this->visual_objects_[index]->model.transform = MatrixScale(scale.x, scale.y, scale.z);
}

void Visualizer::remove_visual_object(int index)
{
    this->visual_objects_.erase(this->visual_objects_.begin() + index);
}

void Visualizer::clear_visual_objects()
{
    this->visual_objects_.clear();
}

void Visualizer::clear_gui_interfaces()
{
    this->imgui_interfaces_calls.clear();
}


// Functions to draw geometric primitives
int Visualizer::add_box(Vector3 position, Quaternion orientation, Color color, float width, float height, float length, int group_id)
{
    Mesh cube_mesh = GenMeshCube(width, height, length);
    Model cube = LoadModelFromMesh(cube_mesh);
    std::shared_ptr<VisualObject> cube_vis_object = std::make_shared<VisualObject>(VisualObject{
        .position = position,
        .orientation = orientation,
        .model = cube,
        .color = color,
        .group_id = group_id});

    return this->add_visual_object(cube_vis_object);
}

int Visualizer::add_sphere(Vector3 position, Quaternion orientation, Color color, float radius, int group_id)
{
    Mesh sphere_mesh = GenMeshSphere(radius, 16, 16);
    Model sphere = LoadModelFromMesh(sphere_mesh);
    std::shared_ptr<VisualObject> sphere_vis_object = std::make_shared<VisualObject>(VisualObject{
        .position = position,
        .orientation = orientation,
        .model = sphere,
        .color = color,
        .group_id = group_id});

    return this->add_visual_object(sphere_vis_object);
}

int Visualizer::add_cylinder(Vector3 position, Quaternion orientation, Color color, float radius, float height, int group_id)
{
    Mesh cylinder_mesh = GenMeshCylinder(radius, height, 16);

    Model cylinder = LoadModelFromMesh(cylinder_mesh);
    cylinder.transform = MatrixMultiply(MatrixTranslate(0, -height * 0.5, 0), MatrixRotateX(PI / 2));
    std::shared_ptr<VisualObject> cylinder_vis_object = std::make_shared<VisualObject>(VisualObject{
        .position = position,
        .orientation = orientation,
        .model = cylinder,
        .color = color,
        .group_id = group_id});

    return this->add_visual_object(cylinder_vis_object);
}

int Visualizer::add_cone(Vector3 position, Quaternion orientation, Color color, float radius, float height, int group_id)
{
    Mesh cone_mesh = GenMeshCone(radius, height, 16);
    Model cone = LoadModelFromMesh(cone_mesh);
    std::shared_ptr<VisualObject> cone_vis_object = std::make_shared<VisualObject>(VisualObject{
        .position = position,
        .orientation = orientation,
        .model = cone,
        .color = color,
        .group_id = group_id});

    return this->add_visual_object(cone_vis_object);
}

int Visualizer::add_plane(Vector3 position, Quaternion orientation, Color color, float width, float length, int group_id)
{
    Mesh plane_mesh = GenMeshPlane(width, length, 16, 16);
    Model plane = LoadModelFromMesh(plane_mesh);
    std::shared_ptr<VisualObject> plane_vis_object = std::make_shared<VisualObject>(VisualObject{
        .position = position,
        .orientation = orientation,
        .model = plane,
        .color = color,
        .group_id = group_id});

    if (this->shader_loaded_)
    {
        this->shadow_texture = LoadRenderTexture(400, 400);
        plane_vis_object->model.materials[0].maps[RL_SHADER_LOC_MAP_ALBEDO].texture = this->shadow_texture.texture;
    }

    return this->add_visual_object(plane_vis_object);
}

int Visualizer::add_mesh(const char *filename, Vector3 position, Quaternion orientation, Color color, float scale, int group_id)
{
    Model model = LoadModel(filename);
    model.transform = MatrixScale(scale, scale, scale);
    std::shared_ptr<VisualObject> vis_object = std::make_shared<VisualObject>(VisualObject{
        .position = position,
        .orientation = orientation,
        .model = model,
        .color = color,
        .group_id = group_id});

    return this->add_visual_object(vis_object);
}


int Visualizer::add_mesh(const char *filename, Vector3 position, Quaternion orientation, Color color, float scale_x, float scale_y, float scale_z, int group_id)
{
    Model model = LoadModel(filename);
    model.transform = MatrixScale(scale_x, scale_y, scale_z);
    std::shared_ptr<VisualObject> vis_object = std::make_shared<VisualObject>(VisualObject{
        .position = position,
        .orientation = orientation,
        .model = model,
        .color = color,
        .group_id = group_id});

    return this->add_visual_object(vis_object);
}

int Visualizer::add_heightmap(Vector3 position,
                              Quaternion orientation,
                              Color color,
                              size_t width,
                              size_t height,
                              const std::vector<float> &data,
                              float x_scale,
                              float y_scale,
                              float z_scale,
                              int group_id)
{

    Image formated_data{
        .data = reinterpret_cast<void *>(const_cast<float *>(data.data())),
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PixelFormat::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};

    Vector3 size_vector = {x_scale, y_scale, z_scale};

    Mesh mesh = GenMeshHeightmap(formated_data, size_vector);

    int x_shift = width / 2;
    int y_shift = height / 2;

    Model model = LoadModelFromMesh(mesh);

    model.transform = MatrixMultiply(MatrixTranslate(-x_shift, 0, -y_shift), MatrixRotateX(PI / 2));

    std::shared_ptr<VisualObject> vis_object = std::make_shared<VisualObject>(VisualObject{
        .position = position,
        .orientation = orientation,
        .model = model,
        .color = color,
        .group_id = group_id});

    return this->add_visual_object(vis_object);
};
