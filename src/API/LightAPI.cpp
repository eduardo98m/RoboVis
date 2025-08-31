
#include "Components/Base.hpp"
#include "ECS/Entity.hpp"
#include "VisualizerNew.hpp"

namespace rbvs {

Entity Visualizer::create_light(LightParams params) {

  Light light = {.position = params.position,
                 .direction = params.direction,
                 .type = params.type,
                 .color = params.color,
                 .intensity = params.intensity,
                 .cut_off = params.cut_off,
                 .outer_cut_off = params.outer_cut_off,
                 .enabled = params.enabled};

  Entity e = this->entity_manager.create();
  this->entity_manager.addComponent<Light>(e, std::move(light));
  return e;
}

void Visualizer::update_light(LightUpdateParams params) {
  Light &lg = this->entity_manager.getComponent<Light>(params.entity);

  if (params.position)
    lg.position = *params.position;
  if (params.direction)
    lg.direction = *params.direction;
  if (params.type)
    lg.type = *params.type;
  if (params.color)
    lg.color = *params.color;
  if (params.intensity)
    lg.intensity = *params.intensity;
  if (params.cut_off)
    lg.cut_off = *params.cut_off;
  if (params.outer_cut_off)
    lg.outer_cut_off = *params.outer_cut_off;
  if (params.enabled)
    lg.enabled = *params.enabled;
}

} // namespace rbvs
