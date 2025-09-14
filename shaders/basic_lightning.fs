#version 430

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0; // diffuse texture keyword, DrawMesh uses this
uniform vec4 colDiffuse;
uniform int u_LightCount;
uniform vec4 ambient_light;
uniform vec3 viewPos;


// Shadow map
uniform sampler2D shadowMapTex;

in vec2 fragShadowTexCoord;
in float fragShadowDepth;


// Output fragment color
out vec4 finalColor;

// NOTE: Add your custom variables here

#define     MAX_LIGHTS              8

// ENUM
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1
#define     LIGHT_SPOT              2

// Light structure - IMPORTANTE: std430 layout
struct Light {
    int enabled;        // 4 bytes
    int type;           // 4 bytes  
    float padding1;     // 4 bytes
    float padding2;     // 4 bytes
    vec4 position;      // 16 bytes
    vec4 target;        // 16 bytes
    vec4 color;         // 16 bytes
    float intensity;    // 4 bytes
    float cutoff;       // 4 bytes (para spot lights)
    float outerCutoff;  // 4 bytes
    float padding3;     // 4 bytes - completar a múltiplo de 16
    // Total: 80 bytes por light (múltiplo de 16)
};


layout(std430, binding = 0)  buffer LightBuffer {
    Light lights[];
};

vec3 calculateDirectionalLight(const Light light, vec3 normal, vec3 viewDir, vec3 objectColor) {
    vec3 lightDir = -normalize(light.target.xyz - light.position.xyz); // <-- use target - position
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * light.color.rgb * light.intensity;
    
    return (diffuse * objectColor) + specular;
}

vec3 calculatePointLight(const Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 objectColor) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    float distance = length(light.position.xyz - fragPos);
    
    // Attenuation
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity * attenuation;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * light.color.rgb * light.intensity * attenuation;
    
    return (diffuse * objectColor) + specular;
}

vec3 calculateSpotLight(const Light light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 objectColor) {
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    float distance = length(light.position.xyz - fragPos);
    
    // Attenuation
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
    // Spotlight intensity
    vec3 spotDir = normalize(light.target.xyz - light.position.xyz); // <-- direction from pos→target
    float theta = dot(lightDir, -spotDir);  // lightDir points to frag, so compare with -spotDir
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.intensity * attenuation * intensity;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * light.color.rgb * light.intensity * attenuation * intensity;
    
    return (diffuse * objectColor) + specular;
}

float calculateShadowPCF(vec2 shadowCoord, float currentDepth) {
    float shadow = 0.0;
    // The size of the grid we sample. A larger size gives softer shadows but is more expensive.
    int pcfSamples = 3; 
    // How far apart the samples are. This controls the "spread" of the softness.
    float texelSize = 1.0 / textureSize(shadowMapTex, 0).x;
    float spread = texelSize * 1.5;

    for (int x = -pcfSamples; x <= pcfSamples; x++) {
        for (int y = -pcfSamples; y <= pcfSamples; y++) {
            // Get the depth from the shadow map at the offset coordinate
            float pcfDepth = texture(shadowMapTex, shadowCoord + vec2(x, y) * spread).r; 
            
            // Add to the average if the fragment is not in shadow
            // The bias (0.001) is still important here to prevent shadow acne
            if (currentDepth - 0.001 < pcfDepth) {
                shadow += 1.0;
            }
        }
    }
    
    // Divide by the total number of samples to get the average (a value between 0.0 and 1.0)
    int totalSamples = (2 * pcfSamples + 1) * (2 * pcfSamples + 1);
    return shadow / float(totalSamples);
}

void main() {
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec4 tint = colDiffuse * fragColor;
    vec3 objectColor = texelColor.rgb * tint.rgb;
    
    // Ambient lighting
    vec3 ambient = ambient_light.rgb * objectColor;
    vec3 lighting = vec3(0.0);
    
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);
    
    // Process all lights
    int lightCount = min(u_LightCount, lights.length());    
    for (int i = 0; i < lightCount; i++) {
        if (lights[i].enabled == 1) {
            switch (lights[i].type) {
                case LIGHT_DIRECTIONAL:
                    lighting += calculateDirectionalLight(lights[i], normal, viewDir, objectColor);
                    break;
                case LIGHT_POINT:
                    lighting += calculatePointLight(lights[i], fragPosition, normal, viewDir, objectColor);
                    break;
                case LIGHT_SPOT:
                    lighting += calculateSpotLight(lights[i], fragPosition, normal, viewDir, objectColor);
                    break;
            }
        }
    }
    
    // Determine shadow factor (1.0 = fully lit, 0.0 = fully shadowed)
    float shadow = 1.0;

    // Only attempt to sample if shadow texcoord is inside [0,1] range
    bool insideShadowMap = (fragShadowTexCoord.x >= 0.0 && fragShadowTexCoord.x <= 1.0 &&
                            fragShadowTexCoord.y >= 0.0 && fragShadowTexCoord.y <= 1.0 &&
                            fragShadowDepth >= 0.0 && fragShadowDepth <= 1.0);

    if (insideShadowMap) {
        // Compute a bias based on rough angle between normal and light direction to reduce acne/leaking.
        // Assume the main shadow caster is lights[0] — if you render multiple shadow maps you'd adapt this per-light.
        vec3 lightDirForBias = vec3(0.0);
        if (lightCount > 0) {
            // for directional lights use target-position, for point/spot approximate with position->frag
            if (lights[0].type == LIGHT_DIRECTIONAL) {
                lightDirForBias = -normalize(lights[0].target.xyz - lights[0].position.xyz);
            } else {
                lightDirForBias = normalize(lights[0].position.xyz - fragPosition);
            }
        } else {
            lightDirForBias = normalize(vec3(0.0, -1.0, 0.0));
        }

        float nl = max(dot(normal, lightDirForBias), 0.0);
        // bias grows when angle is grazing (small dot) — tune these constants if needed
        float bias = max(0.002 * (1.0 - nl), 0.0005);

        // Use PCF to compute how much is lit (your calculateShadowPCF returns fraction lit)
        shadow = calculateShadowPCF(fragShadowTexCoord, fragShadowDepth - bias);
        shadow = clamp(shadow, 0.0, 1.0);
    } else {
        // outside shadow map: treat as lit to avoid ghost shadows
        shadow = 1.0;
    }

    // Combine (keep ambient unshadowed)
    vec3 linearColor = ambient + lighting * shadow;

    // Gamma correction (linear -> sRGB)
    finalColor = pow(vec4(linearColor, texelColor.a), vec4(1.0/2.2));

    
}
