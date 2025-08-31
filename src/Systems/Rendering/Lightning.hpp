#pragma once
#include "ECS/EntityManager.hpp"
#include <raylib.h>

#define MAX_LIGHTS 100 // Make sure this number maches the shader
namespace rbvs
{

  struct GpuLight
  {
    int enabled;       // Aligned to 4 bytes
    int type;          // Aligned to 4 bytes
    float padding1;    // Explicit padding to align `position` to 16-byte boundary
    float padding2;    //
    Vector4 position;  // Aligned to 16 bytes
    Vector4 direction; // Aligned to 16 bytes
    Vector4 color;     // Aligned to 16 bytes
    float intensity;   // Aligned to 4 bytes
    float cutoff;      // Aligned to 4 bytes
    float outerCutoff; // Aligned to 4 bytes
    float padding3;    // To make total size a multiple of 16
  };

  // We will use one single SSBO to store all the lights
  struct LightingGPUUniforms
  {
    Shader litShader;   // Shared shader with lighting

    // Common uniforms (not part of the SSBO)
    int locLightCount = -1;
    int locViewPos = -1;
    int locAmbient = -1;

    // SSBO ID for the light data
    unsigned int ssboId = 0;
  };

  /*
   * @ brief Special type of system that gets passed onto other rendering systems
   * to enable the rendering of lights
   * */
  class LightingSystem
  {

  public:
    /*
     * @brief Initialization of the rendering system called only once
     **/
    void init();

    /**
     * @brief Executed each frame to pass the lights info to the shader
     **/
    void upload_frame_uniforms(EntityManager &em, const Camera3D &camera);

    /*
     * @brief Helper function to update the uniforms for a single light
     * */
    void update_light_values(EntityManager &em, const Camera3D &camera);

    /*
     * @brief Accessor for the lit Shader
     * */
    const Shader &lit_shader() const { return lit.litShader; }

  private:
    LightingGPUUniforms lit;
  };

} // namespace rbvs
