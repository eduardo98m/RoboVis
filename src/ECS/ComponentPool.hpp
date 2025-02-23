#pragma once

#include <vector>
#include <unordered_map>
#include "Entity.hpp"

namespace rbvs
{

    /**
     * @brief Interface for type-erased component pools.
     *
     * This interface allows the EntityManager to manage component pools of various types uniformly.
     */
    class IComponentPool
    {
    public:
        virtual ~IComponentPool() = default;

        /**
         * @brief Removes the component associated with the given entity.
         *
         * @param e The entity whose component is to be removed.
         */
        virtual void remove(Entity e) = 0;

        /**
         * @brief Checks if the component pool has a component for the given entity.
         *
         * @param e The entity to check.
         * @return true if the entity has a component in this pool, false otherwise.
         */
        virtual bool has(Entity e) const = 0;

        /**
         * @brief Returns a vector of entities that have a component in this pool.
         *
         * @return const std::vector<Entity>& A reference to the vector of entities.
         */
        virtual const std::vector<Entity> &entities() const = 0;
    };

    /**
     * @brief Dense, contiguous storage for a single component type.
     *
     * The ComponentPool class template stores components of type T in a dense array to improve cache locality.
     * It maintains bidirectional mappings between entities and indices in the component array.
     *
     * @tparam T The type of component stored in the pool.
     */
    template <typename T>
    class ComponentPool : public IComponentPool
    {
    private:
        std::vector<T> data;                              ///< Dense storage for components.
        std::unordered_map<Entity, size_t> entityToIndex; ///< Maps an entity to its index in the data vector.
        std::vector<Entity> indexToEntity;                ///< Maps an index in the data vector back to the corresponding entity.

    public:
        /**
         * @brief Adds a component for the given entity.
         *
         * If the entity already has a component of this type, the function does nothing.
         *
         * @param e The entity to add the component for.
         * @param component The component to add.
         */
        void add(Entity e, T &&component)
        {
            if (entityToIndex.find(e) != entityToIndex.end())
                return;
            entityToIndex[e] = data.size();
            indexToEntity.push_back(e);
            data.push_back(std::move(component));
        }

        /**
         * @brief Retrieves a reference to the component associated with the given entity.
         *
         * @param e The entity whose component is to be retrieved.
         * @return T& A reference to the component.
         */
        T &get(Entity e)
        {
            return data[entityToIndex.at(e)];
        }

        /**
         * @brief Removes the component associated with the given entity.
         *
         * This method maintains the density of the component array by swapping the component to be removed
         * with the last element in the vector and then popping the back.
         *
         * @param e The entity whose component is to be removed.
         */
        void remove(Entity e) override
        {
            if (!has(e))
                return;
            size_t index = entityToIndex[e];
            // Swap with the last element to maintain density
            data[index] = std::move(data.back());
            data.pop_back();
            Entity lastEntity = indexToEntity.back();
            entityToIndex[lastEntity] = index;
            indexToEntity[index] = lastEntity;
            indexToEntity.pop_back();
            entityToIndex.erase(e);
        }

        /**
         * @brief Checks if the pool has a component for the given entity.
         *
         * @param e The entity to check.
         * @return true if the entity has a component in this pool, false otherwise.
         */
        bool has(Entity e) const override
        {
            return entityToIndex.find(e) != entityToIndex.end();
        }

        /**
         * @brief Returns a vector of entities that have a component in this pool.
         *
         * @return const std::vector<Entity>& A reference to the vector of entities.
         */
        const std::vector<Entity> &entities() const override
        {
            return indexToEntity;
        }
    };

} // namespace rbvs
