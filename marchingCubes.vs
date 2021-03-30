#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec3 stepDimension;
uniform sampler3D density;

//out vec3 fColor;
out vec3 TexCoords;
out int varIndex;

void main()
{
    gl_Position = vec4(aPos, 1.0);

    /*
    // Test: get coordinate components from vertexID
    int id = gl_VertexID;
    int x = id/(96*256);
    int y = (id%(96*256))/256;
    int z = id%256;

    int i = gl_VertexID % 3;
    float x = float(i == 0);
    float y = float(i == 1);
    float z = float(i == 2);

    TexCoords = vec3(x, y, z);
    */

    TexCoords = aPos * stepDimension;
    //fColor = texture(density, TexCoords).rgb;

    int b1 = int(texture(density, TexCoords).r < 0.5f);
    int b2 = int(texture(density, TexCoords + vec3(stepDimension.x, 0.0, 0.0)).r < 0.5f);
    int b3 = int(texture(density, TexCoords + vec3(stepDimension.x, 0.0, stepDimension.z)).r < 0.5f);
    int b4 = int(texture(density, TexCoords + vec3(0.0, 0.0, stepDimension.z)).r < 0.5f);
    int b5 = int(texture(density, TexCoords + vec3(0.0, stepDimension.y, 0.0)).r < 0.5f);
    int b6 = int(texture(density, TexCoords + vec3(stepDimension.x, stepDimension.y, 0.0)).r < 0.5f);
    int b7 = int(texture(density, TexCoords + vec3(stepDimension.x, stepDimension.y, stepDimension.z)).r < 0.5f);
    int b8 = int(texture(density, TexCoords + vec3(0.0, stepDimension.y, stepDimension.z)).r < 0.5f);
    varIndex = (b1 << 7) | (b2 << 6) | (b3 << 5) | (b4 << 4) | (b5 << 3) | (b6 << 2) | (b7 << 1) | b8;
}