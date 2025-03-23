#include <raygizmo.h>
#include "Components/Base.hpp"
#include "ECS/EntityManager.hpp"
#include <imgui.h>
#include <rlImGui.h>
#include<string>


namespace rbvs
{
    class GizmoRenderingSystem
    {
    public:
        void update(EntityManager &em);

        void render_settings_gui();
    private:
        float line_width = 2.5;
        float size = 1.5;
        Color axis_colors[4]  = {{229, 72, 91, 255}, // xAxis
                                 {131, 205, 56, 255}, // y axis
                                 {69, 138, 242, 255}, // z_axis
                                 {255, 255, 255, 200}}; // Center
        
        bool translation_enabled;
        bool rotation_enabled;
        bool scaling_enabled;
        bool global_mode;

        int flags = (GIZMO_ROTATE | GIZMO_TRANSLATE | GIZMO_LOCAL);

    };

}
