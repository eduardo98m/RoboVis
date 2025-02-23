#include "Systems/Rendering/GUIRendering.hpp"

void render_gui(std::vector<std::function<void(void)>> imgui_interfaces_calls)
{
    rlImGuiBegin();
    for (auto &imgui_interface_function : imgui_interfaces_calls)
    {
        imgui_interface_function();
    }
    rlImGuiEnd();
};