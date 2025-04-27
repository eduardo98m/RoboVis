#include "Systems/Rendering/GUIRendering.hpp"

void render_gui(const std::map<std::string, std::function<void(void)>> &user_guis)
{
    
    for (const auto& [name, func] : user_guis) {
        func(); // Call the function
    }
    
    
};