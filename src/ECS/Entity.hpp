#pragma once
// STD
#include <cstdint>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>

namespace rbvs {

/// Maximum number of distinct components an entity can have.
constexpr uint32_t MAX_COMPONENTS = 64;

/**
 * @brief Represents an entity in the ECS.
 *
 * An entity is represented by a unique id and a generation counter.
 * The generation counter is used to ensure that stale entity handles (from previously destroyed entities)
 * are not mistakenly used.
 */
struct Entity {
    uint32_t id;           ///< Unique identifier for the entity.
    uint32_t generation;   ///< Generation counter to invalidate stale entity references.

    /**
     * @brief Compares two entities for equality.
     *
     * Two entities are equal if they have the same id and generation.
     *
     * @param e The other entity to compare with.
     * @return true if the entities are equal, false otherwise.
     */
    bool operator==(const Entity& e) const { 
        return id == e.id && generation == e.generation; 
    }
};

} // namespace rbvs

// Specialize std::hash for rbvs::Entity so it can be used in unordered_map.
namespace std {
    template<> struct hash<rbvs::Entity> {
        /**
         * @brief Generates a hash value for an Entity.
         *
         * The hash is computed by combining the hash of the entity's id and its generation.
         *
         * @param e The entity to hash.
         * @return size_t The computed hash value.
         */
        size_t operator()(const rbvs::Entity& e) const {
            return hash<uint32_t>()(e.id) ^ (hash<uint32_t>()(e.generation) << 1);
        }
    };
}
