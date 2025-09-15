#include "Systems/Rendering/ShadowMapRendering.hpp"
#include "Components/Base.hpp"
#include "ECS/EntityManager.hpp"
// raylib
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
// std
#include <string>

namespace rbvs {
void ShadowMapRenderingSystem::init() {
  this->shadow_map =
      load_shadow_map(shadow_map_resolution, shadow_map_resolution);
}

Camera3D ShadowMapRenderingSystem::create_light_camera(const Light &light) {
  Camera3D lightCam = {0};

  switch (light.type) {  
  case LightType::DIRECTIONAL: {
    // For directional lights, position the camera far away in the opposite
    // direction
    lightCam.position = light.position;
    lightCam.target = light.target; // Look at origin (or adjust based on your scene)
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
    Vector3 lightDir = Vector3Normalize(Vector3Subtract(light.target, light.position));
    Vector3 up = {0.0f, 1.0f, 0.0f};
    if (fabsf(Vector3DotProduct(up, lightDir)) > 0.99f) {
        up = {1.0f, 0.0f, 0.0f};
    }
    lightCam.up = up;
    // lightCam.fovy = shadow_ortho_size; // This controls the size of the
    //                                    // orthographic projection
    break;
  }
  case LightType::POINT: {
    // For point lights, you'd typically need cube shadow mapping (6 faces)
    // This is a simplified approach for one direction
    lightCam.position = light.position;
    lightCam.target = light.target; // We need to change this
    lightCam.projection = CAMERA_PERSPECTIVE;
    lightCam.up = (Vector3){0.0f, 1.0f, 0.0f};
    //lightCam.fovy = 90.0f; // Wide field of view for point lights
    break;
  }
  case LightType::SPOT: {
    lightCam.position = light.position;
    lightCam.target = light.target;
    lightCam.projection = CAMERA_PERSPECTIVE;
    lightCam.up = (Vector3){0.0f, 1.0f, 0.0f};
    break;
  }
  }

  return lightCam;
}

void ShadowMapRenderingSystem::begin_shadow_map(RenderTexture2D target,
                                                const Light &light,
                                              Shader lightning_shader) {
  BeginTextureMode(target);
  ClearBackground({0, 0, 0, 0});

  Camera3D lightCam = create_light_camera(light);
  BeginMode3D(lightCam);
  // The BeginMode3D automatically sets up the projection and view matrices
  double width = this->shadow_ortho_size;
  double height = this->shadow_ortho_size;
  Matrix lightProj = MatrixOrtho(-width / 2, width / 2, -height / 2,
                                    height / 2, this->shadow_near,
                                    this->shadow_far);
  rlSetMatrixProjection(lightProj);
  Matrix lightView = GetCameraMatrix(lightCam);
  int locLightVP = GetShaderLocation(lightning_shader, "matLightVP");
  Matrix lightVP = MatrixMultiply(lightView, lightProj);
  SetShaderValueMatrix(lightning_shader, locLightVP, lightVP);

}

void ShadowMapRenderingSystem::end_shadow_map() {
  EndMode3D();
  EndTextureMode();
}

void ShadowMapRenderingSystem::unload_shadow_map(RenderTexture2D target) {
  if (target.id > 0) {
    rlUnloadFramebuffer(target.id);
  }
}

RenderTexture2D ShadowMapRenderingSystem::load_shadow_map(int width,
                                                          int height) {
  RenderTexture2D target = {0};

  target.id = rlLoadFramebuffer();
  if (target.id > 0) {
    rlEnableFramebuffer(target.id);
    target.texture.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    target.texture.width = width;
    target.texture.height = height;
    target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    target.texture.mipmaps = 1;

    target.depth.id = rlLoadTextureDepth(width, height, false);
    target.depth.width = width;
    target.depth.height = height;
    target.depth.format =
        PIXELFORMAT_UNCOMPRESSED_R32; // DEPTH_COMPONENT_24BIT?
    target.depth.mipmaps = 1;

    // Filterign is not necessary but makes the shadow look better
    rlTextureParameters(target.depth.id, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_LINEAR);
    rlTextureParameters(target.depth.id, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_LINEAR);
    
    rlTextureParameters(target.depth.id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
    rlTextureParameters(target.depth.id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);
    

    rlFramebufferAttach(target.id, target.texture.id,
                        RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D,
                        0);
    rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH,
                        RL_ATTACHMENT_TEXTURE2D, 0);                      
    rlDisableFramebuffer();
  }
  return target;
}

void ShadowMapRenderingSystem::update(EntityManager &em, const Camera3D &camera,
                                      Shader lightning_shader) {

  auto view = em.view<Light>();
  for (auto e : view) {
    const auto &light = em.getComponent<Light>(e);
    if (!light.enabled && light.type != LightType::DIRECTIONAL)
      continue;

    this->begin_shadow_map(this->shadow_map, light, lightning_shader);

    auto model_view = em.view<VisualModel>();
    for (auto model_entity : model_view) {
      auto &vm = em.getComponent<VisualModel>(model_entity);

      if (!vm.visible || !vm.receive_lighting || vm.color.a < 255) {
        continue;
      }
      Vector3 axis;
      float angle;
      QuaternionToAxisAngle(vm.orientation, &axis, &angle);
      DrawModelEx(*vm.model, vm.position, axis, angle, vm.scale, WHITE);
    }
    // Upload the data to the lightning shader
    int locShadowMap = GetShaderLocation(lightning_shader, "shadowMapTex");
    SetShaderValueTexture(lightning_shader, locShadowMap, this->shadow_map.depth);
    this->end_shadow_map();
    rlActiveTextureSlot(1);
    rlEnableTexture(shadow_map.depth.id);

    break;
  }
}
} // namespace rbvs
