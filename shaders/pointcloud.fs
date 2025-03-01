#version 430

in vec4 fragColor;
in vec3 fragNormal;

out vec4 finalColor;

void main() {
    // Simple lighting calculation
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diffuse = max(dot(fragNormal, lightDir), 0.2);
    
    // Final color combines the point color with lighting
    finalColor = vec4(fragColor.rgb * diffuse, fragColor.a);
}