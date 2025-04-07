#version 430

// Input vertex attributes from vertex shader
in vec2 fragTexCoord;
in vec3 fragPosition;
in vec3 fragNormal;

// Output fragment color
out vec4 finalColor;

// Uniform variables
uniform sampler2D heights;

void main()
{
    // Normalized height for color gradient
    float normalizedHeight = texture(heights, fragTexCoord).r;
    
    // Calculate base color based on height
    // Lower elevations: darker green
    // Middle elevations: light brown
    // Higher elevations: white (snow)
    vec3 lowColor = vec3(0.0, 0.5, 0.0);
    vec3 midColor = vec3(0.6, 0.4, 0.2);
    vec3 highColor = vec3(1.0, 1.0, 1.0);
    
    vec3 baseColor;
    if (normalizedHeight < 0.3) {
        // Interpolate between low and mid color
        float t = normalizedHeight / 0.3;
        baseColor = mix(lowColor, midColor, t);
    } else if (normalizedHeight < 0.7) {
        // Interpolate between mid and high color
        float t = (normalizedHeight - 0.3) / 0.4;
        baseColor = mix(midColor, highColor, t);
    } else {
        // High terrain
        baseColor = highColor;
    }
    
    // Simple lighting calculation
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diffuse = max(dot(fragNormal, lightDir), 0.2);
    
    // Combine colors with lighting
    finalColor = vec4(1, 0, 1, 1);//vec4(baseColor * diffuse, 1.0);
}
