/**********************************************************************************************
*
*   raylib.lights - Some useful functions to deal with lights data
*   Modified to work with RoboVis in C++ by Eduardo Lopez 2023 (github : eduardo98m)
*
*   CONFIGURATION:
*
*   #define RLIGHTS_IMPLEMENTATION
*       Generates the implementation of the library into the included file.
*       If not defined, the library is in header only mode and can be included in other headers 
*       or source files without problems. But only ONE file should hold the implementation.
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2017-2023 Victor Fisac (@victorfisac) and Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/


#pragma once
#include "raylib.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_LIGHTS  4         // Max dynamic lights supported by shader


//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Light data
typedef struct {   
    int type;          /**< Type of the light. */
    bool enabled;       /**< Whether the light is enabled. */
    Vector3 position;   /**< Position of the light. */
    Vector3 target;     /**< Target of the light. */
    Color color;        /**< Color of the light. */
    float attenuation;  /**< Attenuation factor of the light. */
    
    // Shader locations
    int enabledLoc;       /**< Shader location for the enabled flag. */
    int typeLoc;          /**< Shader location for the light type. */
    int positionLoc;      /**< Shader location for the light position. */
    int targetLoc;        /**< Shader location for the light target. */
    int colorLoc;         /**< Shader location for the light color. */
    int attenuationLoc;   /**< Shader location for the light attenuation. */
} Light;

// Light type
typedef enum {
    LIGHT_DIRECTIONAL = 0,  /**< Directional light type. */
    LIGHT_POINT             /**< Point light type. */
} LightType;


//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

/**
 * @brief Creates a light and obtains shader locations for its properties.
 *
 * @param type The type of the light (LIGHT_DIRECTIONAL or LIGHT_POINT).
 * @param position The position of the light.
 * @param target The target of the light.
 * @param color The color of the light.
 * @param shader The shader to which the light properties will be sent.
 * @return Light The created light structure.
 */
Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader);   // Create a light and get shader locations


/**
 * @brief Updates the shader with the properties of the given light.
 *
 * @param shader The shader to update.
 * @param light The light structure containing the properties.
 */
void UpdateLightValues(Shader shader, Light light);         // Send light properties to shader