#include "Systems/GUI/EntitySettings.hpp"

namespace rbvs
{
    void EntitySettingsSystem::render_gui(EntityManager &em)
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_Modal;
        // ImGui::Begin("Main Window", nullptr, window_flags);

        ImGui::Begin("Entities", nullptr, window_flags);
        ImGui::Text("Visualizer");
        auto view = em.view<VisualModel>();

        // Loop through each entity with a VisualModel component.
        for (auto entity : view)
        {
            VisualModel &vm = em.getComponent<VisualModel>(entity);
            // Ensure a unique ID for ImGui widgets (push/pop id)
            ImGui::PushID(entity.id);
            
            // Create a collapsing header per entity.
            std::string headerLabel = "Entity " + std::to_string(entity.id);
            if (ImGui::CollapsingHeader(headerLabel.c_str()))
            {
                // Checkbox to toggle visibility.
                ImGui::Checkbox("Visible", &vm.visible);

                // Use a color picker to modify the entity's color.
                // Convert your Color to a float array.
                float color[4] = {
                    vm.color.r / 255.0f,
                    vm.color.g / 255.0f,
                    vm.color.b / 255.0f,
                    vm.color.a / 255.0f
                };
                if (ImGui::ColorEdit4("Color", color))
                {
                    // Update the Raylib Color by converting from [0,1] to [0,255].
                    vm.color.r = static_cast<unsigned char>(color[0] * 255.0f);
                    vm.color.g = static_cast<unsigned char>(color[1] * 255.0f);
                    vm.color.b = static_cast<unsigned char>(color[2] * 255.0f);
                    vm.color.a = static_cast<unsigned char>(color[3] * 255.0f);
                }
            }

            ImGui::PopID();
        }

        ImGui::End();
    }
}