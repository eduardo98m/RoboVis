#version 430

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform int u_LightCount;
uniform vec4 ambient_light;
uniform vec3 viewPos;

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
    int type;          // 4 bytes  
    float padding1;    // 4 bytes - para alineación
    float padding2;    // 4 bytes - para alineación
    vec4 position;     // 16 bytes
    vec4 direction;    // 16 bytes
    vec4 color;        // 16 bytes
    float intensity;   // 4 bytes
    float cutoff;      // 4 bytes (para spot lights)
    float outerCutoff; // 4 bytes
    float padding3;    // 4 bytes - completar a múltiplo de 16
    // Total: 80 bytes por light (múltiplo de 16)
};


layout(std430, binding = 0)  buffer LightBuffer {
    Light lights[];
};

vec3 calculateDirectionalLight(const Light light, vec3 normal, vec3 viewDir, vec3 objectColor) {
    vec3 lightDir = normalize(-light.direction.xyz);
    
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
    float theta = dot(lightDir, normalize(-light.direction.xyz));
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
    
    vec3 result = ambient + lighting;
    finalColor = vec4(result, texelColor.a);
    
    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
}