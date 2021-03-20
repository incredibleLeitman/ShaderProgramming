#version 330 core

//uniform sampler3D density;

in vec3 TexCoordsG;
//in vec3 fColor;

out vec4 FragColor;

void main()
{
     //float density = vec4(texture(density, TexCoordsG)).r;
     FragColor = vec4(TexCoordsG, 1);
}