#include "Systems/Rendering/MapRendering.hpp"

namespace rbvs
{

  void MapRenderingSystem::update(EntityManager &em, const Camera3D &camera)
  {
    auto view = em.view<Map>();

    for (auto entity : view)
    {
      auto &map = em.getComponent<Map>(entity);

      if (!map.visible)
        continue;

      this->update_chunks(map, camera.position);

      Vector3 axis;
      float angle;
      QuaternionToAxisAngle(map.orientation, &axis, &angle);

      const Color lod_map[4] = {RED, ORANGE, YELLOW, GREEN};
      Color lod_map_alpha[4] = {{RED.r, RED.g, RED.b, 30},
                                {ORANGE.r, ORANGE.g, ORANGE.b, 30},
                                {YELLOW.r, YELLOW.g, YELLOW.b, 30},
                                {GREEN.r, GREEN.g, GREEN.b, 30}};
      for (const auto &key : map.visible_chunks)
      {
        auto model_it = map.loaded_chunks.find(key);
        if (model_it == map.loaded_chunks.end())
          continue;

        Model *model = model_it->second.get();
        const ChunkInfo &chunk = map.chunks[key];

        DrawModelEx(*model, map.position, axis, angle, map.scale,
                    lod_map[key.lod]);

        // DrawModelWiresEx(*model,
        //                  map.position, axis, angle,
        //                  map.scale, lod_map_alpha[key.lod]);

        Vector3 size = map.chunk_size;
        Vector3 position = map.position + chunk.position;

        // rlDisableDepthMask();
        // DrawCubeV(position, size, lod_map_alpha[key.lod]);
        // rlEnableDepthMask();

        DrawCubeWiresV(position, size, lod_map[key.lod]);
      }
    }
  }

  void MapRenderingSystem::update_chunks(Map &map, const Vector3 &camera_pos)
  {
    map.visible_chunks.clear(); // Reset visible set

    Vector3 camera_local_pos = camera_pos - map.position;

    camera_local_pos =
        Vector3RotateByQuaternion(camera_local_pos, map.orientation);

    for (const auto &[id, chunk] : map.chunks)
    {
      float distance = Vector3Distance(camera_local_pos, chunk.position);
      if (distance > rendering_distance)
        continue;

      float step = rendering_distance / map.lods;
      size_t lod = std::min(static_cast<size_t>(distance / step), map.lods - 1);

      ChunkKey key = id;
      key.lod = lod;

      if (map.loaded_chunks.find(key) == map.loaded_chunks.end())
      {
        map.loaded_chunks[key] =
            std::make_unique<Model>(LoadModel(chunk.model_path.c_str()));
      }

      map.visible_chunks.insert(key);
    }
  }

} // namespace rbvs
