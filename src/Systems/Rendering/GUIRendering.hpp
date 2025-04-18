#pragma once
// Imgui
#include "imgui.h"
// Raylib
#include "rlImGui.h"
// Standard Library
#include <functional>
#include <map>
#include <string>

void render_gui(const std::map<std::string, std::function<void(void)>> &user_guis);