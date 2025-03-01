#version 430

// Input vertex position (unit square centered at origin)
layout (location=0) in vec3 vertexPosition;

// Uniforms
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float particleScale;

// SSBO containing point positions
layout(std430, binding=0) buffer ssbo0 { vec3 positions[]; };

// Output
out vec4 fragColor;
//out vec3 fragNormal;

void main() {
    // Get the point position for this instance
    vec3 pointPosition = positions[gl_InstanceID];//.xyz;
    
    // Compute world-space position of the point
    vec4 worldPoint = modelMatrix * vec4(pointPosition, 1.0);

    // Extract the camera's right and up vectors from the view matrix
    vec3 cameraRight = normalize(vec3(viewMatrix[0].xyz)); // Right direction
    vec3 cameraUp = normalize(vec3(viewMatrix[1].xyz));    // Up direction

    // Compute billboard vertex position
    vec3 billboardVertex = worldPoint.xyz + vertexPosition * particleScale;
                          //+ (vertexPosition.x * cameraRight + vertexPosition.y * cameraUp) * particleScale;

    // Final position in clip space
    gl_Position = projectionMatrix * viewMatrix * vec4(billboardVertex, 1.0);

    // Use a constant normal (facing the camera)
    //fragNormal = vec3(0.0, 0.0, 1.0);

    // Color based on world position
    vec3 normalizedPos = normalize(pointPosition) * 0.5 + 0.5;
    fragColor = vec4(normalizedPos, 1.0);
}
