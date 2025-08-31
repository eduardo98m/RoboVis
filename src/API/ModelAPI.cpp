#include "VisualizerNew.hpp"
#include <raylib.h>
#include <raymath.h>
#include <string>
namespace rbvs
{

  void TransformMesh(Mesh *mesh, Matrix transform)
  {
    // Vertex
    for (int i = 0; i < mesh->vertexCount; i++)
    {
      Vector3 pos = {mesh->vertices[i * 3 + 0], mesh->vertices[i * 3 + 1],
                     mesh->vertices[i * 3 + 2]};
      pos = Vector3Transform(pos, transform);
      mesh->vertices[i * 3 + 0] = pos.x;
      mesh->vertices[i * 3 + 1] = pos.y;
      mesh->vertices[i * 3 + 2] = pos.z;
    }

    // Rotate normals
    if (mesh->normals)
    {
      Matrix normalMatrix = transform;
      normalMatrix.m12 = normalMatrix.m13 = normalMatrix.m14 =
          0.0f; // quitar traslación
      for (int i = 0; i < mesh->vertexCount; i++)
      {
        Vector3 norm = {mesh->normals[i * 3 + 0], mesh->normals[i * 3 + 1],
                        mesh->normals[i * 3 + 2]};
        norm = Vector3Transform(norm, normalMatrix);
        norm = Vector3Normalize(norm);
        mesh->normals[i * 3 + 0] = norm.x;
        mesh->normals[i * 3 + 1] = norm.y;
        mesh->normals[i * 3 + 2] = norm.z;
      }
    }

    if (mesh->vaoId == 0)
    {
      // If mesh hasn't been uploaded to GPU yet, upload it now
      UploadMesh(mesh, false);
    }
    else
    {
      // If mesh already exists on GPU, update the vertex buffer
      UpdateMeshBuffer(*mesh, 0, mesh->vertices,
                       mesh->vertexCount * 3 * sizeof(float), 0);

      // Update normals buffer if it exists
      if (mesh->normals)
      {
        UpdateMeshBuffer(*mesh, 2, mesh->normals,
                         mesh->vertexCount * 3 * sizeof(float), 0);
      }
    }
  }

  void Visualizer::register_model(std::string model_name,
                                  std::vector<ModelPrimitive> primitives)
  {
    if (this->registered_models.count(model_name))
    {
      std::cerr << "Warning: model already registered with name " << model_name
                << " \n";
    }

    // Material and meshses vetors (would like to pre-allocate memory to this)
    std::vector<Material> materials;
    std::vector<Mesh> meshes;

    meshes.reserve(primitives.size());
    materials.reserve(primitives.size());

    for (const auto primitive : primitives)
    {
      Mesh mesh;
      Material material = LoadMaterialDefault();
      switch (primitive.type)
      {
      case ModelPrimitiveType::SPHERE:
        mesh = GenMeshSphere(primitive.radius, 32, 32);
        break;
      case ModelPrimitiveType::BOX:
        mesh = GenMeshCube(primitive.half_extents.x * 2.f,
                           primitive.half_extents.y * 2.f,
                           primitive.half_extents.z * 2.f);
        break;
      case ModelPrimitiveType::CYLINDER:
        mesh = GenMeshCylinder(primitive.radius, primitive.height, 32);
        break;
      case ModelPrimitiveType::CONE:
        mesh = GenMeshCone(primitive.radius, primitive.height, 32);
        break;
      case ModelPrimitiveType::HEMISPHERE:
        mesh = GenMeshHemiSphere(primitive.radius, 32, 32);
        break;
      case ModelPrimitiveType::TORUS:
        mesh = GenMeshTorus(primitive.radius, primitive.size, 32, 32);
        break;
      case ModelPrimitiveType::POLYGON:
      {
        if (primitive.sides < 3)
          std::cerr << "Warning, triying to create polygon with less than 3 "
                       "sides -> mesh wont be created";
        mesh = GenMeshPoly(primitive.sides, primitive.radius);
        break;
      }
      default:
        std::cerr << "Warning: Unsupported primitive type in register_model.\n";
        continue;
      }
      Matrix transform_matrix = MatrixMultiply(
          QuaternionToMatrix(primitive.orientation),
          MatrixTranslate(primitive.position.x, primitive.position.y,
                          primitive.position.z));
      TransformMesh(&mesh, transform_matrix);

      material.maps[MaterialMapIndex::MATERIAL_MAP_DIFFUSE].color =
          primitive.color;
      materials.push_back(material);
      meshes.push_back(mesh);
    }

    Model model{};
    model.transform = MatrixTranslate(0.0, 0.0, 0.0);
    model.meshCount = static_cast<int>(meshes.size());
    model.materialCount = static_cast<int>(materials.size());

    std::vector<Mesh> tmpMeshes = meshes;           // Copy from your existing vector
    std::vector<Material> tmpMaterials = materials; // Copy materials
    std::vector<int> tmpMeshMaterial(model.meshCount);

    for (int i = 0; i < model.meshCount; ++i)
      tmpMeshMaterial[i] = i;

    // Allocate Raylib-owned memory
    model.meshes = static_cast<Mesh *>(MemAlloc(sizeof(Mesh) * model.meshCount));
    model.materials =
        static_cast<Material *>(MemAlloc(sizeof(Material) * model.materialCount));
    model.meshMaterial =
        static_cast<int *>(MemAlloc(sizeof(int) * model.meshCount));

    // Copy to Raylib's struct
    std::memcpy(model.meshes, tmpMeshes.data(), sizeof(Mesh) * model.meshCount);
    std::memcpy(model.materials, tmpMaterials.data(),
                sizeof(Material) * model.materialCount);
    std::memcpy(model.meshMaterial, tmpMeshMaterial.data(),
                sizeof(int) * model.meshCount);
    this->registered_models[model_name] = model;
  };

  Entity Visualizer::create_model(ModelParams params)
  {

    Mesh mesh;

    VisualModel vm = {.position = params.position,
                      .orientation = params.orientation,
                      .scale = params.scale,
                      .color = params.color,
                      .receive_lighting = params.receive_lighting};

    // GenMeshCylinder(radius, height, 16);
    switch (params.model_type)
    {
    case ModelType::SPHERE:
      vm.model = std::make_unique<Model>(
          LoadModelFromMesh(this->pre_loaded_meshes.sphere_mesh));
      vm.model->transform =
          MatrixScale(params.radius, params.radius, params.radius);
      break;
    case ModelType::CYLINDER:
      vm.model = std::make_unique<Model>(
          LoadModelFromMesh((this->pre_loaded_meshes.cylinder_mesh)));
      vm.model->transform =
          MatrixScale(params.radius, params.length, params.radius);
      break;
    case ModelType::BOX:
      vm.model = std::make_unique<Model>(
          LoadModelFromMesh(this->pre_loaded_meshes.box_mesh));
      vm.model->transform = MatrixScale(
          params.half_extents.x, params.half_extents.y, params.half_extents.z);
      break;
    case ModelType::CONE:
      vm.model = std::make_unique<Model>(
          LoadModelFromMesh(this->pre_loaded_meshes.cone_mesh));
      vm.model->transform =
          MatrixScale(params.radius, params.length, params.radius);
      break;
    case ModelType::MESH:
      vm.model = std::make_unique<Model>(LoadModel(params.model_path.c_str()));
      break;
    case ModelType::CUSTOM:
    {
      if (this->registered_models.count(params.custom_model_key))
      {
        vm.model = std::make_unique<Model>(
            this->registered_models[params.custom_model_key]);
      }
      else
      {
        std::cerr << "Model " << params.custom_model_key
                  << " is not a registered model";
      }
      break;
    }
    default:
      break;
    }

    // Assing the correct shader to the model
    if (vm.model && vm.receive_lighting)
    {
      for (int i = 0; i < vm.model->materialCount; i++)
      {
        vm.model->materials[i].shader = this->rendering_system.lighting_system.lit_shader();
      }
    }

    Entity e = this->entity_manager.create();
    this->entity_manager.addComponent<VisualModel>(e, std::move(vm));

    return e;
  }

  void Visualizer::update_model(ModelUpdateParams params)
  {
    // TODO : We need a way to avoid getting empty components (i.e. the entity
    // does not have the component)
    VisualModel &vm =
        this->entity_manager.getComponent<VisualModel>(params.entity);

    if (params.position)
      vm.position = *params.position;
    if (params.orientation)
      vm.orientation = *params.orientation;
    if (params.scale)
      vm.scale = *params.scale;
    if (params.color)
      vm.color = *params.color;
    if (params.visible)
      vm.visible = *params.visible;
    if (params.receive_lighting)
    {
      vm.receive_lighting = *params.receive_lighting;
      if (vm.receive_lighting)
      {
        for (int i = 0; i < vm.model->materialCount; i++)
        {
          vm.model->materials[i].shader = this->rendering_system.lighting_system.lit_shader();
        }
      }
      else
      {
        Material mat = LoadMaterialDefault(); // TODO : This might be somwhat of a hack but it works ._.
        for (int i = 0; i < vm.model->materialCount; i++)
        {
          vm.model->materials[i].shader = mat.shader;
        }
      }
    }
  }

  void Visualizer::delete_model(Entity entity)
  {
    // TODO : We need a way to avoid getting empty components (i.e. the entity
    // doenst have the component)
    this->entity_manager.destroy(entity);
  }

  void Visualizer::pre_load_meshes(void)
  {
    this->pre_loaded_meshes.box_mesh = GenMeshCube(1.0, 1.0, 1.0);
    this->pre_loaded_meshes.sphere_mesh = GenMeshSphere(1.0, 64, 64);
    this->pre_loaded_meshes.cylinder_mesh = GenMeshCylinder(1.0, 1.0, 64);
    this->pre_loaded_meshes.cone_mesh = GenMeshCone(1.0, 1.0, 64);
  }
} // namespace rbvs
