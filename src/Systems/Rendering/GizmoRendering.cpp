#include "Systems/Rendering/GizmoRendering.hpp"

namespace rbvs
{

    void GizmoRenderingSystem::update(EntityManager &em){

        auto view = em.view<Gizmo>();

        SetGizmoSize(this->size);
        SetGizmoLineWidth(this->line_width);
        SetGizmoColors(
            this->axis_colors[0],
            this->axis_colors[1],
            this->axis_colors[2],
            this->axis_colors[3]);


        for (auto entity : view){

            // Get the pointcloud entuty from the entity manager
            auto &gizmo = em.getComponent<Gizmo>(entity);

            // Check if it is visible and if it has points
            if (!gizmo.visible)
                continue;
            
            // It is possible to introduce some snapping using this kinds of functions
            //gizmo.transform.translation.x = float(int(gizmo.transform.translation.x));

            DrawGizmo3D(this->flags, &gizmo.transform);
            
        }
    };

    void GizmoRenderingSystem::render_settings_gui(){
        ImGui::Begin("Gizmo Settings");


        ImGui::CheckboxFlags("Scaling", &this->flags, GIZMO_SCALE);
        ImGui::CheckboxFlags("Rotation", &this->flags, GIZMO_ROTATE);
        ImGui::CheckboxFlags("Translation", &this->flags, GIZMO_TRANSLATE);
        ImGui::CheckboxFlags("Local", &this->flags, GIZMO_LOCAL);

        ImGui::Separator();
        ImGui::SliderFloat("Size",&this->size, 0.0, 5.0);
        ImGui::SliderFloat("Linze Width",&this->line_width, 0.0, 5.0);
        std::string labels[4] = {"X axis", "Y axis", "Z axis", "Center"};
        if (ImGui::CollapsingHeader("Axis colors")){
            for (int i =0; i<4; i++) {
                Color &c  = axis_colors[i];
                float color[4] = {
                    c.r / 255.0f,
                    c.g / 255.0f,
                    c.b / 255.0f,
                    c.a / 255.0f
                };
                std::string label = labels[i] + " color";
                if (ImGui::ColorEdit4(label.c_str(), color))
                {
                    // Update the Raylib Color by converting from [0,1] to [0,255].
                    c.r = static_cast<unsigned char>(color[0] * 255.0f);
                    c.g = static_cast<unsigned char>(color[1] * 255.0f);
                    c.b = static_cast<unsigned char>(color[2] * 255.0f);
                    c.a = static_cast<unsigned char>(color[3] * 255.0f);
                }
    
            }
        }
        ImGui::End();
    }

}
