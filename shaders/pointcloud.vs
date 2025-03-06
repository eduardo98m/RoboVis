#version 430

// Input vertex position (unit square centered at origin)
layout (location=0) in vec3 vertexPosition;

// Uniforms
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float particleScale;
uniform bool billboarding = false;
uniform int colorMode = 0;  // New uniform for color mode
uniform vec4 pointColor;  // New uniform for point color
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;



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

    vec3 billVert = (vertexPosition.x * CameraRight_worldspace + 
                           vertexPosition.y * CameraUp_worldspace) * particleScale;

    vec3 regVert = vertexPosition * particleScale;

    vec3 vert  = billboarding ? billVert : regVert;
    
    // Compute billboard vertex position
    vec3 billboardVertex = worldPoint.xyz + vert; 
                          //billVert * ;

    // Final position in clip space
    gl_Position = projectionMatrix * viewMatrix * vec4(billboardVertex, 1.0);

    // Use a constant normal (facing the camera)
    //fragNormal = vec3(0.0, 0.0, 1.0);

    // Color based on world position
    vec3 normalizedPos = normalize(pointPosition) * 0.5 + 0.5;
    
    if (colorMode == 1) {
        fragColor = vec4(normalizedPos.x, normalizedPos.y, normalizedPos.z, 1.0);  // Color by Axis X
    } else if (colorMode == 2) {
        fragColor = vec4(normalizedPos.y, normalizedPos.z, normalizedPos.x, 1.0);  // Color by Axis Y
    } else if (colorMode == 3) {
        fragColor = vec4(normalizedPos.z, normalizedPos.x, normalizedPos.y, 1.0);  // Color by Axis Z
    } else {
        fragColor = pointColor;//vec4(normalizedPos, 1.0);  // Default color
    }

    //fragColor = vec4(normalizedPos, 1.0);  // Default color

}
