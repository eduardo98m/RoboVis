/**
 * @brief 
 */
#pragma once
#include <raylib.h>
#include <iostream>
#include <memory>

// Basic componentes
namespace rbvs
{
    struct VisualModel
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        std::unique_ptr<Model> model;
    };


    struct DebugShape
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        float lifetime;
    };

    struct TextLabel
    {
        Vector3 position;
        Quaternion orientation;
        Vector3 scale;
        Color color;
        float lifetime;
    };
        
}
