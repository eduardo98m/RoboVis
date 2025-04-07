#pragma once
// External
#include <raylib.h>
#include <raymath.h>
#include "imgui.h"
#include "rlImGui.h"
#include "rlights.hpp"
#include "rlgl.h"
// Standar Library
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <iostream>
// Internal
#include "ECS/Component.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EntityManager.hpp"
// Internal - Systems
#include "Systems/Rendering/Rendering.hpp"
#include "Systems/UserCameraInput.hpp"
#include "Systems/GUI/EntitySettings.hpp"

// Internal - API
#include "API/Model.hpp"
#include "API/PointCloud.hpp"
#include "API/HeightMap.hpp"
#include "API/Gizmo.hpp"

namespace rbvs
{
    class Visualizer {
    private:

        EntityManager entity_manager;
        RenderingSystem rendering_system;
        PreLoadedMeshes pre_loaded_meshes;
        UserCameraInputSystem camera_control_system;
        Camera3D camera;
        void set_up_camera(void);
        void pre_load_meshes(void);
        
    public:
        Visualizer(int screen_width, int screen_height, const char *title);

        ~Visualizer();

        void update();
        
        
        /**
         * @brief Creates a model entity
         */
        Entity create_model(ModelParams params);
        /**
         * @brief Updates a model entity
         */
        void update_model(ModelUpdateParams params);

        void delete_model(Entity entity);

        Entity create_point_cloud(PointCloudParams params);

        void update_point_cloud(PointCloudUpdateParams params);

        void delete_point_cloud(Entity entity);

        Entity create_gizmo(GizmoParams params);

        void update_gizmo(GizmoUpdateParams params);

        Entity create_height_map(HeightMapParams params);

        void update_height_map(HeightMapUpdateParams params);

        //void delete_entity(Entity entity);

        /**
         * @brief  
         * */
        void close();

        
    };
};