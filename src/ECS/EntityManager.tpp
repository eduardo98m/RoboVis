#pragma once
#include "ECS/EntityManager.hpp"
#include <tuple>



namespace rbvs
{

    template <typename T>
    void EntityManager::addComponent(Entity e, T &&component)
    {
        auto type = std::type_index(typeid(T));
        // Create a new component pool if one doesn't already exist for type T.
        if (componentPools.find(type) == componentPools.end())
        {
            componentPools[type] = std::make_unique<ComponentPool<T>>();
        }
        auto pool = static_cast<ComponentPool<T> *>(componentPools[type].get());
        pool->add(e, std::forward<T>(component));
        // Update the entity's signature by setting the bit corresponding to T.
        signatures[e].set(ComponentType<T>::id());
    }

    template <typename T>
    T &EntityManager::getComponent(Entity e)
    {
        auto type = std::type_index(typeid(T));
        auto pool = static_cast<ComponentPool<T> *>(componentPools[type].get());
        return pool->get(e);
    }

    template <typename... Ts>
    std::vector<Entity> EntityManager::view()
    {
        std::vector<Entity> result;

        // Create a mask for the required component types.
        std::bitset<MAX_COMPONENTS> requiredMask;
        // Fold expression to set bits for each component type in Ts.
        ((requiredMask.set(ComponentType<Ts>::id())), ...);

        // For a simple implementation, we use the pool of the first component type as our candidate set.
        // More advanced implementations might iterate over the smallest pool for better performance.
        using FirstType = typename std::tuple_element<0, std::tuple<Ts...>>::type;
        auto firstTypeIndex = std::type_index(typeid(FirstType));

        // If no pool exists for the first type, then no entity has that component.
        if (componentPools.find(firstTypeIndex) == componentPools.end())
        {
            return result;
        }

        auto pool = static_cast<ComponentPool<FirstType> *>(componentPools[firstTypeIndex].get());
        const auto &candidateEntities = pool->entities();

        // Check each candidate entity to see if its signature includes all required components.
        for (Entity e : candidateEntities)
        {
            if ((signatures[e] & requiredMask) == requiredMask)
            {
                result.push_back(e);
            }
        }

        return result;
    }

} // namespace rbvs
