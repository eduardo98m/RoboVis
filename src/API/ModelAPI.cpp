#include "VisualizerNew.hpp"

namespace rbvs
{
    Entity Visualizer::create_model(ModelParams params)
    {

        Mesh mesh;

        VisualModel vm = {
            .position = params.position,
            .orientation = params.orientation,
            .scale = params.scale,
            .color = params.color,
        };

        // GenMeshCylinder(radius, height, 16);
        switch (params.model_type)
        {
        case ModelType::SPHERE:
            //mesh = GenMeshSphere(params.radius, 32, 32);
            vm.model = std::make_unique<Model>(LoadModelFromMesh(this->pre_loaded_meshes.sphere_mesh));
            vm.model->transform = MatrixScale(params.radius, params.radius, params.radius);
            break;
        case ModelType::CYLINDER:
            //mesh = GenMeshCylinder(params.radius, params.length, 32);
            vm.model = std::make_unique<Model>(LoadModelFromMesh((this->pre_loaded_meshes.cylinder_mesh)));
            vm.model->transform = MatrixScale(params.radius, params.length, params.radius);
            break;
        case ModelType::BOX:
            //mesh = GenMeshCube(params.half_extents.x, params.half_extents.y, params.half_extents.z);
            vm.model = std::make_unique<Model>(LoadModelFromMesh(this->pre_loaded_meshes.box_mesh));
            vm.model->transform = MatrixScale(params.half_extents.x, params.half_extents.y, params.half_extents.z);
            break;
        case ModelType::CONE:
            //mesh = GenMeshCone(params.radius, params.length, 32);
            vm.model = std::make_unique<Model>(LoadModelFromMesh(this->pre_loaded_meshes.cone_mesh));
            vm.model->transform = MatrixScale(params.radius, params.length, params.radius);
            break;
        case ModelType::MESH:
            vm.model = std::make_unique<Model>(LoadModel(params.model_path.c_str()));
            break;
        default:
            break;
        }

        Entity e = this->entity_manager.create();
        this->entity_manager.addComponent<VisualModel>(e, std::move(vm));

        return e;
    }


    void Visualizer::update_model(ModelUpdateParams params)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doenst have the component)
        VisualModel &vm = this->entity_manager.getComponent<VisualModel>(params.entity);
        
        if (params.position) vm.position = *params.position;
        if (params.orientation) vm.orientation = *params.orientation;
        if (params.scale) vm.scale = *params.scale;
        if (params.color) vm.color = *params.color; 
        if (params.visible) vm.visible = *params.visible;      
    }

    void Visualizer::delete_model(Entity entity)
    {
        // TODO : We need a way to avoid getting empty components (i.e. the entity doenst have the component)
        this->entity_manager.destroy(entity);   
    }

    void Visualizer::pre_load_meshes(void)
    {
        this->pre_loaded_meshes.box_mesh = GenMeshCube(1.0, 1.0, 1.0);
        this->pre_loaded_meshes.sphere_mesh = GenMeshSphere(1.0, 64, 64);
        this->pre_loaded_meshes.cylinder_mesh = GenMeshCylinder(1.0, 1.0, 64);
        this->pre_loaded_meshes.cone_mesh = GenMeshCone(1.0, 1.0, 64);    
    }
}
