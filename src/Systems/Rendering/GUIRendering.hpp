#pragma once
// Raylib
#include "rlImGui.h"
// Standard Library
#include <functional>
#include <vector>

void render_gui(std::vector<std::function<void(void)>> imgui_interfaces_calls);