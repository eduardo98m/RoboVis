#include "Systems/Rendering/Lightning.hpp"
#include "Components/Base.hpp"
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <rlgl.h>

namespace rbvs
{

  void LightingSystem::init()
  {
    std::string light_vs_path =
        std::string(SHADER_BASE_PATH) + "/basic_lightning.vs";
    std::string light_fs_path =
        std::string(SHADER_BASE_PATH) + "/basic_lightning.fs";

    std::string unlit_vs_path = std::string(SHADER_BASE_PATH) + "/unlit.vs";
    std::string unlit_fs_path = std::string(SHADER_BASE_PATH) + "/unlit.fs";

    lit.litShader = LoadShader(light_vs_path.c_str(), light_fs_path.c_str());

    // Cache uniform locations (only those not in the SSBO)
    lit.locLightCount = GetShaderLocation(lit.litShader, "u_LightCount");
    lit.locViewPos = GetShaderLocation(lit.litShader, "viewPos");
    lit.locAmbient = GetShaderLocation(lit.litShader, "ambient_light");

    // Default ambient light
    float ambient[4] = {0.25f, 0.25f, 0.25f, 1.0f};
    SetShaderValue(lit.litShader, lit.locAmbient, ambient, SHADER_UNIFORM_VEC4);
  }

  void LightingSystem::update_light_values(EntityManager &em,
                                           const Camera3D &camera)
  {
    std::vector<GpuLight> lights_data;
    auto view = em.view<Light>();
    for (auto e : view)
    {
      const auto &light = em.getComponent<Light>(e);
      if (!light.enabled)
        continue;
      GpuLight data;
      data.enabled = 1;
      data.type = static_cast<int>(light.type);
      data.intensity = light.intensity;
      data.cutoff = light.cut_off;
      data.outerCutoff = light.outer_cut_off;
      data.position = {light.position.x, light.position.y, light.position.z, 1.0f};
      Vector3 d = light.direction;
      if (Vector3Length(d) > 0.0001f)
        d = Vector3Normalize(d);
      data.direction = {d.x, d.y, d.z, 0.0f};
      data.color = {light.color.r / 255.0f, light.color.g / 255.0f,
                    light.color.b / 255.0f, light.color.a / 255.0f};
      lights_data.push_back(data);

      if (lights_data.size() == MAX_LIGHTS) // Limit to avoid a lot of ligths
        break;
    }

    int count = (int)lights_data.size();

    // Set the uniform for the light count
    if (lit.locLightCount != -1)
    {
      SetShaderValue(lit.litShader, lit.locLightCount, &count,
                     SHADER_UNIFORM_INT);
    }

    // SSBBO
    if (lit.ssboId == 0)
    {
      if (!lights_data.empty())
      {
        lit.ssboId = rlLoadShaderBuffer(lights_data.size() * sizeof(GpuLight), lights_data.data(), RL_DYNAMIC_DRAW);
        TraceLog(LOG_INFO, "SSBO for lights created with ID: %d", lit.ssboId);
      }
    }
    else
    {
      rlUpdateShaderBuffer(lit.ssboId, lights_data.data(), lights_data.size() * sizeof(GpuLight), 0);
    }

    // Binding the SSBO
    rlBindShaderBuffer(lit.ssboId, 0);

    // View (camera) position
    if (lit.locViewPos != -1)
    {
      const float viewPos[3] = {camera.position.x, camera.position.y,
                                camera.position.z};
      SetShaderValue(lit.litShader, lit.locViewPos, viewPos, SHADER_UNIFORM_VEC3);
    }

    // Warn if critical uniforms are missing
    if (lit.locViewPos == -1 || lit.locLightCount == -1)
    {
      TraceLog(LOG_WARNING, "Lighting: some uniform locations were not found. "
                            "Ensure names match GLSL.");
    }
  }

} // namespace rbvs
