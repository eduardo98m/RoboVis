#version 430

// Input vertex position (unit square centered at origin)
layout (location=0) in vec3 vertexPosition;

// Uniforms
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float particleScale;
uniform bool billboarding = false;
uniform int colorMode = 0;
uniform vec4 pointColor;
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform float maxDrawDistance = 100;
uniform vec3 cameraPosition;

// SSBO containing point positions
layout(std430, binding=0) buffer ssbo0 { vec3 positions[]; };

// Output
out vec4 fragColor;

// Constants for culling
const float CULL_VALUE = 1e10;



// Frustum planes in world space
// These will be calculated in the shader
vec4 frustumPlanes[6];

// Function to extract frustum planes from the view-projection matrix
void extractFrustumPlanes(mat4 mvp) {
    // Left plane
    frustumPlanes[0].x = mvp[0][3] + mvp[0][0];
    frustumPlanes[0].y = mvp[1][3] + mvp[1][0];
    frustumPlanes[0].z = mvp[2][3] + mvp[2][0];
    frustumPlanes[0].w = mvp[3][3] + mvp[3][0];
    
    // Right plane
    frustumPlanes[1].x = mvp[0][3] - mvp[0][0];
    frustumPlanes[1].y = mvp[1][3] - mvp[1][0];
    frustumPlanes[1].z = mvp[2][3] - mvp[2][0];
    frustumPlanes[1].w = mvp[3][3] - mvp[3][0];
    
    // Bottom plane
    frustumPlanes[2].x = mvp[0][3] + mvp[0][1];
    frustumPlanes[2].y = mvp[1][3] + mvp[1][1];
    frustumPlanes[2].z = mvp[2][3] + mvp[2][1];
    frustumPlanes[2].w = mvp[3][3] + mvp[3][1];
    
    // Top plane
    frustumPlanes[3].x = mvp[0][3] - mvp[0][1];
    frustumPlanes[3].y = mvp[1][3] - mvp[1][1];
    frustumPlanes[3].z = mvp[2][3] - mvp[2][1];
    frustumPlanes[3].w = mvp[3][3] - mvp[3][1];
    
    // Near plane
    frustumPlanes[4].x = mvp[0][3] + mvp[0][2];
    frustumPlanes[4].y = mvp[1][3] + mvp[1][2];
    frustumPlanes[4].z = mvp[2][3] + mvp[2][2];
    frustumPlanes[4].w = mvp[3][3] + mvp[3][2];
    
    // Far plane
    frustumPlanes[5].x = mvp[0][3] - mvp[0][2];
    frustumPlanes[5].y = mvp[1][3] - mvp[1][2];
    frustumPlanes[5].z = mvp[2][3] - mvp[2][2];
    frustumPlanes[5].w = mvp[3][3] - mvp[3][2];
    
    // Normalize all planes
    for (int i = 0; i < 6; i++) {
        float len = length(vec3(frustumPlanes[i].x, frustumPlanes[i].y, frustumPlanes[i].z));
        frustumPlanes[i] /= len;
    }
}

// Function to test if a point is inside the frustum
bool isPointInFrustum(vec3 point, float scale, float margin) {
    for (int i = 0; i < 6; i++) {

        float signedDistance = dot(vec4(point, 1.0), frustumPlanes[i]);

        if (signedDistance < -(margin + scale)) {
            return false;
        }
    }
    return true;
}

bool isWithinDrawDistance(vec3 point, vec3 cameraPos, float maxDistance) {
    return distance(point, cameraPos) <= maxDistance;
}
    

void main() {
    // Get the point position for this instance
    vec3 pointPosition = positions[gl_InstanceID];
    
    // Check if this point has been culled by the compute shader
    // if (pointPosition.x > CULL_VALUE * 0.5) {
    //     // This point has been culled, so move it off-screen
    //     gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
    //     fragColor = vec4(0.0);
    //     return;
    // }
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
    
    // Extract frustum planes
    extractFrustumPlanes(mvp);
    

    // Compute world-space position of the point
    vec4 worldPoint = modelMatrix * vec4(pointPosition, 1.0);


    if (!isWithinDrawDistance(worldPoint.xyz, cameraPosition, maxDrawDistance) || !isPointInFrustum(worldPoint.xyz, 0.0, 5.0)  ){
        gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
        fragColor = vec4(0.0);
        return;
    }

    
    // Determine whether to use billboarding
    vec3 billVert = (vertexPosition.x * CameraRight_worldspace + 
                     vertexPosition.y * CameraUp_worldspace) * particleScale;
    
    vec3 regVert = vertexPosition * particleScale;
    
    vec3 vert = billboarding ? billVert : regVert;
    
    // Compute billboard vertex position
    vec3 billboardVertex = worldPoint.xyz + vert;
    
    // Final position in clip space
    gl_Position = projectionMatrix * viewMatrix * vec4(billboardVertex, 1.0);
    
    // Calculate color based on colorMode
    vec3 normalizedPos = normalize(pointPosition) * 0.5 + 0.5;
    
    // Define a permutation matrix for color assignments
    const int indices[3][3] = int[3][3](
        int[3](0, 1, 2),  // Color by X
        int[3](1, 2, 0),  // Color by Y
        int[3](2, 0, 1)   // Color by Z
    );
    
    // Select the appropriate permutation
    if (colorMode >= 1 && colorMode <= 3) {
        fragColor = vec4(
            normalizedPos[indices[colorMode - 1][0]],
            normalizedPos[indices[colorMode - 1][1]],
            normalizedPos[indices[colorMode - 1][2]],
            1.0
        );
    } else {
        fragColor = pointColor;  // Default solid color
    }
}