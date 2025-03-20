// We require version 430 since it supports compute shaders.
// THis file is left as a reference on how to use custom compute shaders but it is 
// unnecesesary as it functionlity can be covered in the vertex shader
#version 430

// This is the workgroup size.
layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

// Input and output buffer for point positions
layout(std430, binding=0) buffer ssbo0 { vec3 positions[]; };

// Uniform inputs
layout(location=0) uniform float draw_distance = 100;
layout(location=1) uniform float particleScale;
layout(location=2) uniform vec3 cameraPosition;
layout(location=3) uniform mat4 projectionMatrix;
layout(location=4) uniform mat4 viewMatrix;
layout(location=5) uniform mat4 modelMatrix;

const float cullingMargin = 5.0; // Margin as a fraction of the frustum size

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


void main() {
    // Calculate the view-projection matrix
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
    
    // Extract frustum planes
    extractFrustumPlanes(mvp);
    
    // Get the index of the current point
    uint index = gl_GlobalInvocationID.x;
    
    // Safety check to avoid out-of-bounds access
    if (index >= positions.length()) {
        return;
    }
    
    // Get the current point position
    vec3 pos = positions[index];
    
    // Transform the point to world space
    vec4 worldPos = modelMatrix * vec4(pos, 1.0);
    vec3 worldPoint = worldPos.xyz / worldPos.w;
    
    // Check if the point is within the frustum and draw distance
    // 

    float scaled_scale = min(particleScale/distance(worldPos, cameraPosition), particleScale * 0.0);
    //|| !isWithinDrawDistance(worldPos.xyz, cameraPosition, draw_distance)
    if (!isPointInFrustum(worldPoint, scaled_scale, cullingMargin)  ) {
        // If not, move the point far away (effectively culling it)
        // We'll use a special value that the vertex shader can detect
        // Use a position that's far outside the frustum
        positions[index] = vec3(1e10, 1e10, 1e10);
    }
    
    // If the point is visible, keep its original position
    // No need to modify positions[index] in this case
}