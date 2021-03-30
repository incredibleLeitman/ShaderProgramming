#version 430
layout (location = 0) in vec3 aPos;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords = vec2(aPos.x, aPos.y);
}