#include "ECS/EntityManager.hpp"

namespace rbvs {

Entity EntityManager::create() {
    uint32_t id = freeList.empty() ? entities.size() : freeList.back();
    if (freeList.empty()) {
        // No free ID available; add a new entity.
        entities.push_back(Entity{id, 0});
    } else {
        // Reuse an ID from the free list and increment its generation.
        freeList.pop_back();
        entities[id].generation++;
    }
    return entities[id];
}

void EntityManager::destroy(Entity e) {
    // Validate that the entity is still current.
    if (e.id >= entities.size() || entities[e.id].generation != e.generation)
        return;

    // Remove components from all pools that have this entity.
    for (auto& [type, pool] : componentPools) {
        if (pool->has(e))
            pool->remove(e);
    }

    // Erase the entity's signature.
    signatures.erase(e);

    // Add the entity ID to the free list for reuse.
    freeList.push_back(e.id);
}

} // namespace rbvs
