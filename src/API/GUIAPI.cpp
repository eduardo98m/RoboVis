
#include "VisualizerNew.hpp"

namespace rbvs
{

    bool Visualizer::add_gui(std::string name, std::function<void(void)> gui_function)
    {
        auto [it, inserted] = user_guis.emplace(std::move(name), std::move(gui_function));
        return inserted;
    }

    bool Visualizer::remove_gui(std::string name)
    {
        return user_guis.erase(name) > 0;
    }
}