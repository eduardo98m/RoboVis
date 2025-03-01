#version 430

// Input vertex position (from the cube geometry)
layout (location=0) in vec3 vertexPosition;

// Uniforms for matrices and configuration
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float particleScale;

// SSBO containing all point positions
layout(std430, binding=0) buffer ssbo0 { vec4 positions[]; };

// Output to fragment shader
out vec4 fragColor;
out vec3 fragNormal;

void main() {
    // Get the point position for this instance
    vec3 pointPosition = positions[gl_InstanceID].xyz;
    
    // Transform the point by the model matrix to get world space position
    vec4 worldPoint = modelMatrix * vec4(pointPosition, 1.0);
    
    // Scale the cube vertex and add it to the point position
    vec3 scaledVertex = vertexPosition * particleScale;
    vec3 worldVertex = worldPoint.xyz + scaledVertex;
    
    // Calculate final position in clip space
    gl_Position = projectionMatrix * viewMatrix * vec4(worldVertex, 1.0);
    
    // Calculate normal (simplified for cube geometry)
    // Since our cube vertices are centered around the origin and have values of -0.5 or 0.5,
    // we can use the vertex position as an approximation of the normal
    fragNormal = normalize(vertexPosition);
    
    // Set color based on position or normal (you can customize this)
    // Here we're using position normalized to [0,1] range for RGB
    vec3 normalizedPos = normalize(pointPosition) * 0.5 + 0.5;
    fragColor = vec4(normalizedPos, 1.0);
}