#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraUp;
uniform vec3 cameraLeft;

in float gSize[];
in vec4 gColor[];
in float gAlive[];

out vec4 fColor;

vec2 quadVertices[] = vec2[](
    vec2(-0.5,-0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5));

// build a quad from 2 triangles
int quadIndices[] = int[](
    0, 1, 2,
    1, 3, 2);

void main()
{
    fColor = gColor[0];
    for (int triangle = 0; triangle < 2; ++triangle)
    {
        for (int index = 0; index < 3; ++index)
        {
            // calculat offset so quads always faces the camera
            vec3 offset = cameraLeft * quadVertices[quadIndices[triangle*3 + index]].x +
                          cameraUp * quadVertices[quadIndices[triangle*3 + index]].y;
            gl_Position = gl_in[0].gl_Position + gAlive[0]*vec4(gSize[0]*offset, 1.0);
            gl_Position = projection * view * model * gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}  