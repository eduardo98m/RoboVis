#pragma once
// std
#include <vector>
#include <unordered_map>
#include <memory>
#include <bitset>
#include <typeindex>
#include <tuple>
// Internal
#include "ECS/Entity.hpp"
#include "ECS/ComponentPool.hpp"
#include "ECS/Component.hpp"

namespace rbvs {

/**
 * @brief Manages entities and their components in the ECS.
 *
 * The EntityManager handles creation and destruction of entities, and provides methods to add and retrieve components.
 */
class EntityManager {
private:
    std::vector<Entity> entities;  ///< Vector of all entities.
    std::vector<uint32_t> freeList;  ///< List of free entity IDs available for reuse.
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> componentPools;  ///< Map of component pools by type.
    std::unordered_map<Entity, std::bitset<MAX_COMPONENTS>> signatures;  ///< Component signatures for each entity.

public:
    /**
     * @brief Creates a new entity.
     *
     * If there are no free entity IDs, a new one is allocated.
     * Otherwise, a free ID is reused and the entity's generation is incremented.
     *
     * @return Entity The newly created entity.
     */
    Entity create();

    /**
     * @brief Destroys an entity.
     *
     * This method removes all components associated with the entity, updates its signature,
     * and adds its ID to the free list for reuse.
     *
     * @param e The entity to destroy.
     */
    void destroy(Entity e);

    /**
     * @brief Adds a component to the given entity.
     *
     * If the component pool for the given type does not exist, it is created.
     * The component is then added to the entity and the entity's signature is updated.
     *
     * @tparam T The type of the component.
     * @param e The entity to add the component to.
     * @param component The component to add.
     */
    template<typename T>
    void addComponent(Entity e, T&& component);

    /**
     * @brief Retrieves a component from the given entity.
     *
     * @tparam T The type of the component.
     * @param e The entity whose component is to be retrieved.
     * @return T& A reference to the component.
     */
    template<typename T>
    T& getComponent(Entity e);

    /**
     * @brief Check if an entity has a certain component
     *
     * @tparam T The type of the component.
     * @param e The entity whose component is being checked
     * @return T& A reference to the component.
     */    
    template<typename T>
    bool hasComponent(Entity e) const;

    /**
     * @brief Returns a list of entities that have all the specified component types.
     *
     * This function creates a view of entities that contain all specified components by constructing a
     * bitmask of required components and iterating over a candidate pool (using the first component type
     * as a starting point). More sophisticated approaches may choose the smallest pool for iteration.
     *
     * @tparam Ts The component types to filter by.
     * @return std::vector<Entity> List of entities with the specified components.
     */
    template<typename... Ts>
    std::vector<Entity> view();
};

} // namespace rbvs

// Include template implementations.
#include "ECS/EntityManager.tpp"
