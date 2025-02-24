/**
 * @brief We describe a Systems that takes all the entities and produces a GUI that allows to change the attributes of the entity
 */


#pragma once
// IMGUI
#include "imgui.h"

// Internal - ECS
#include "ECS/Entity.hpp"
#include "ECS/EntityManager.hpp"

#include "Components/Base.hpp"


#include <string>


namespace rbvs{
class EntitySettingsSystem{
    public:
        void render_gui(EntityManager &em);

};
}//namespace rbvs