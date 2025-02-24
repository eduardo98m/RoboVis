#version 440

// Uniforms for view and projection matrices
uniform mat4 view;
uniform mat4 projection;
uniform vec3 pos;

in vec3 vertexPosition;

layout(location = 1) out vec3 nearPoint;
layout(location = 2) out vec3 farPoint;
layout(location = 3) out mat4 fragView;
layout(location = 7) out mat4 fragProj;

// // Grid positions in XY clipped space
// vec3 gridPlane[6] = vec3[](
//     vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
//     vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
// );

const vec3 Pos[4] = vec3[4](
    vec3(-1.0, -1.0, 0.0),      // bottom left
    vec3( 1.0, -1.0, 0.0),      // bottom right
    vec3( 1.0, 1.0,  0.0),      // top right
    vec3(-1.0, 1.0,  0.0)       // top left
);


const int Indices[6] = int[6](1, 0, 3,
                              2, 3, 0);


vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    //vec3 p = gridPlane[gl_VertexID].xyz;
    int Index = Indices[gl_VertexID];
    vec3 p = Pos[Index];

    nearPoint = UnprojectPoint(p.x, p.y, 0.0, view, projection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, view, projection).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
    fragView = view;
    fragProj = projection;

    gl_Position = vec4(p.xy, p.z + 0.01, 1.0);
}
