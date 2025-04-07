#version 430

// Input vertex attributes
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

// Output vertex attributes
out vec2 fragTexCoord;
out vec3 fragPosition;
out vec3 fragNormal;

// Uniform variables
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D heights;

void main() {
    // Get heightmap dimensions
    ivec2 hmSize = textureSize(heights, 0);
    int hmWidth = hmSize.x;
    int hmHeight = hmSize.y;
    
    // Calculate grid coordinates from vertex ID
    int gridX = gl_VertexID % hmWidth;
    int gridZ = gl_VertexID / hmWidth;
    
    // Calculate texture coordinates
    vec2 texCoord = vec2(float(gridX) / float(hmWidth-1), float(gridZ) / float(hmHeight-1));
    
    // Sample height from the texture
    float height = texture(heights, texCoord).r;
    
    // Create vertex position in model space
    vec3 position = vec3(
        float(gridX) / float(hmWidth-1) - 0.5,  // Map to -0.5 to 0.5 range
        height,
        float(gridZ) / float(hmHeight-1) - 0.5
    );
    
    // Transform vertex position to world space
    vec4 worldPosition = modelMatrix * vec4(position, 1.0);
    
    // Culling check
    //mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
    // extractFrustumPlanes(mvp);
    
    // if (!isWithinDrawDistance(worldPosition.xyz, cameraPosition, maxDrawDistance) || 
    //     !isPointInFrustum(worldPosition.xyz, 0.0, 5.0)) {
    //     // Move vertex off-screen if outside view
    //     gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
    //     return;
    // }
    
    // Calculate normal by sampling neighboring heights
    vec2 texelSize = 1.0 / vec2(hmWidth, hmHeight);
    float heightL = textureOffset(heights, texCoord, ivec2(-1, 0)).r;
    float heightR = textureOffset(heights, texCoord, ivec2(1, 0)).r;
    float heightU = textureOffset(heights, texCoord, ivec2(0, -1)).r;
    float heightD = textureOffset(heights, texCoord, ivec2(0, 1)).r;
    
    // Handle edge cases
    if (gridX == 0) heightL = height;
    if (gridX == hmWidth-1) heightR = height;
    if (gridZ == 0) heightU = height;
    if (gridZ == hmHeight-1) heightD = height;
    
    // Calculate normal using central differences
    vec3 normal = normalize(vec3(
        heightL - heightR,
        2.0, // Scale factor for normal intensity
        heightU - heightD
    ));
    
    // Pass data to fragment shader
    fragPosition = worldPosition.xyz;
    fragNormal = mat3(modelMatrix) * normal;
    fragTexCoord = texCoord;
    
    // Output position in clip space
    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}