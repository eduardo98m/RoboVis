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
    // Total: 80 bytes por light ( 5 x 16)
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

float calculateShadowPCFPoisson(vec2 shadowCoord, float currentDepth, float bias) {
    // Poisson disk samples for better distribution than grid sampling
    vec2 poissonDisk[16] = vec2[](
        vec2(-0.94201624, -0.39906216),
        vec2(0.94558609, -0.76890725),
        vec2(-0.094184101, -0.92938870),
        vec2(0.34495938, 0.29387760),
        vec2(-0.91588581, 0.45771432),
        vec2(-0.81544232, -0.87912464),
        vec2(-0.38277543, 0.27676845),
        vec2(0.97484398, 0.75648379),
        vec2(0.44323325, -0.97511554),
        vec2(0.53742981, -0.47373420),
        vec2(-0.26496911, -0.41893023),
        vec2(0.79197514, 0.19090188),
        vec2(-0.24188840, 0.99706507),
        vec2(-0.81409955, 0.91437590),
        vec2(0.19984126, 0.78641367),
        vec2(0.14383161, -0.14100790)
    );
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMapTex, 0);
    
    // Sample multiple points using Poisson disk
    for(int i = 0; i < 16; i++) {
        vec2 sampleCoord = shadowCoord + poissonDisk[i] * texelSize * 2.0;
        float pcfDepth = texture(shadowMapTex, sampleCoord).r;
        
        if(currentDepth - bias < pcfDepth) {
            shadow += 1.0;
        }
    }
    
    return shadow / 16.0;
}


// PCSS (Percentage Closer Soft Shadows) - even better quality but more expensive
float calculateShadowPCSS(vec2 shadowCoord, float currentDepth, vec3 lightDir) {
    vec2 texelSize = 1.0 / textureSize(shadowMapTex, 0);
    
    // Step 1: Find average blocker depth
    float searchRadius = 5.0 * texelSize.x;
    float blockerDepthSum = 0.0;
    float blockerCount = 0.0;
    
    for(int i = 0; i < 16; i++) {
        vec2 poissonDisk[16] = vec2[](
            vec2(-0.94201624, -0.39906216),
            vec2(0.94558609, -0.76890725),
            vec2(-0.094184101, -0.92938870),
            vec2(0.34495938, 0.29387760),
            vec2(-0.91588581, 0.45771432),
            vec2(-0.81544232, -0.87912464),
            vec2(-0.38277543, 0.27676845),
            vec2(0.97484398, 0.75648379),
            vec2(0.44323325, -0.97511554),
            vec2(0.53742981, -0.47373420),
            vec2(-0.26496911, -0.41893023),
            vec2(0.79197514, 0.19090188),
            vec2(-0.24188840, 0.99706507),
            vec2(-0.81409955, 0.91437590),
            vec2(0.19984126, 0.78641367),
            vec2(0.14383161, -0.14100790)
        );
        
        vec2 sampleCoord = shadowCoord + poissonDisk[i] * searchRadius;
        float sampleDepth = texture(shadowMapTex, sampleCoord).r;
        
        if(sampleDepth < currentDepth) {
            blockerDepthSum += sampleDepth;
            blockerCount += 1.0;
        }
    }
    
    if(blockerCount == 0.0) {
        return 1.0; // No blockers found
    }
    
    float avgBlockerDepth = blockerDepthSum / blockerCount;
    
    // Step 2: Calculate penumbra size
    float lightSize = 0.05; // Adjust this for light size
    float penumbraSize = (currentDepth - avgBlockerDepth) / avgBlockerDepth * lightSize;
    
    // Step 3: PCF with variable filter size
    float shadow = 0.0;
    float filterRadius = penumbraSize * texelSize.x;
    
    for(int i = 0; i < 16; i++) {
        vec2 poissonDisk[16] = vec2[](
            vec2(-0.94201624, -0.39906216),
            vec2(0.94558609, -0.76890725),
            vec2(-0.094184101, -0.92938870),
            vec2(0.34495938, 0.29387760),
            vec2(-0.91588581, 0.45771432),
            vec2(-0.81544232, -0.87912464),
            vec2(-0.38277543, 0.27676845),
            vec2(0.97484398, 0.75648379),
            vec2(0.44323325, -0.97511554),
            vec2(0.53742981, -0.47373420),
            vec2(-0.26496911, -0.41893023),
            vec2(0.79197514, 0.19090188),
            vec2(-0.24188840, 0.99706507),
            vec2(-0.81409955, 0.91437590),
            vec2(0.19984126, 0.78641367),
            vec2(0.14383161, -0.14100790)
        );
        
        vec2 sampleCoord = shadowCoord + poissonDisk[i] * filterRadius;
        float pcfDepth = texture(shadowMapTex, sampleCoord).r;
        
        float bias = max(0.005 * (1.0 - dot(normalize(fragNormal), lightDir)), 0.001);
        
        if(currentDepth - bias < pcfDepth) {
            shadow += 1.0;
        }
    }
    
    return shadow / 16.0;
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
        float bias = max(0.002 * (1.0 - nl), 0.0005);

        shadow = calculateShadowPCFPoisson(fragShadowTexCoord, fragShadowDepth, bias);
        
        shadow = clamp(shadow, 0.0, 1.0);
    } else {
        shadow = 1.0;
    }

    // Combine (keep ambient unshadowed)
    vec3 linearColor = ambient + lighting * shadow;

    // Gamma correction (linear -> sRGB)
    finalColor = pow(vec4(linearColor, texelColor.a), vec4(1.0/2.2));

    
}
