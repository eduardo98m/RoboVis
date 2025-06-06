/**
 * @brief Structs for model entities creation and updating
 * @author Eduardo Lopez : eduardo98m@gmail.com
 * 
 */
#pragma once
// tinyxml
#include <tinyxml2.h>
// Raylib
#include <raylib.h>
// Standart Library
#include <string>
#include <optional>
#include "Components/Base.hpp"
// ECS
#include "ECS/Entity.hpp"
#include <filesystem>
#include <regex>

namespace rbvs
{

    /**
     * @brief Struct holding parameters for creating a Map (using a )
     */
    struct MapParams
    {
        Vector3 position = {0.0f, 0.0f, 0.0f};         ///< Map position in world space.
        Quaternion orientation = {0.0, 0.0, 0.0, 1.0}; ///< Map orientation as a quaternion.
        Vector3 scale = {1.0f, 1.0f, 1.0f};            ///< Scaling factors for the map.
        std::string map_data_path = ""; ///< File path for custom mesh models (used for ModelType::MESH).
    };

    /**
     * @brief Struct for updating model properties dynamically.
     *        Uses std::optional to allow partial updates without allocating heap memory.
     */
    struct MapUpdateParams
    {
        Entity entity; // The entity to which we will be updating the params
        std::optional<Vector3> position{};       ///< Optional new position.
        std::optional<Quaternion> orientation{}; ///< Optional new orientation.
        std::optional<Vector3> scale{};          ///< Optional new scale.
        std::optional<bool> visible{};
    };

} // namespace rbvs
