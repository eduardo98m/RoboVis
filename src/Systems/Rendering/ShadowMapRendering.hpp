#pragma once
#include "Components/Base.hpp"
#include "ECS/EntityManager.hpp"
// raylib
#include <raylib.h>

namespace rbvs {
struct ShadowMapGPUUniforms {
  int light_clip_near = -1;
  int light_clip_far = -1;
  int mvp = -1;
};

class ShadowMapRenderingSystem {

public:
  // Class attriburtes
  RenderTexture2D shadow_map;
  Shader shadow_shader;
  ShadowMapGPUUniforms shadow_shader_uniforms;

  int shadow_map_resolution = 8000; // Quality of the shadows
  float shadow_ortho_size = 50.0f; // Less s
  float shadow_near = 0.1f;
  float shadow_far = 50.0f;

  
  void init();

  /**
   * @brief
   */
  Camera3D create_light_camera(const Light &light);

  /**
   * @brief
   */
  void begin_shadow_map(RenderTexture2D target, const Light &light,  Shader lightning_shader);
  
  /**
   * @brief
   */
  void end_shadow_map();
  
  /**
   * @brief 
   */
  void unload_shadow_map(RenderTexture2D target);

  RenderTexture2D load_shadow_map(int width, int height);

  /** 
   * @brief Renders the shadow maps and uploads them to the GPU
   * It also uploads the textures indices and VP matrix of the light used for
   * the shadows
   * */
  void update(EntityManager &em, const Camera3D &camera,
              Shader lighting_shader);

};
} // namespace rbvs
