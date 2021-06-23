//#version 330 core
#version 430 // version 430 allows array with initializer list

uniform int layer;
uniform float yOffset;
uniform float bufferHeight;

in vec2 TexCoords;

out vec4 FragColor;

vec2 pillars[3] = { vec2(0.2,0.6), vec2(0.7,0.8), vec2(0.6,0.3) };
//vec2 pillars[2] = { vec2(0.2,0.6), vec2(0.7,0.8) };

float CreatePillars(vec2 position, int amount)
{
    float f = 0.0f;
    for (int i = 0; i < amount; i++)
    {
        //f += 1 / (length(position - pillars[i]) * 1.5) - 1;
        f += 0.05/length(position - pillars[i]) - 0.05;
    }
    return f;
}

vec2 RotatePosition(vec2 position, float angle)
{
    mat2 rot;
    rot[0] = vec2(cos(angle), -sin(angle));
    rot[1] = vec2(sin(angle), cos(angle));
    return rot * position;
}

void main()
{
    float density = 0.0f;

    // rotate
    vec2 rotPos = TexCoords;
    //vec2 rotPos = RotatePosition(TexCoords, layer);

    // helix
    //vec2 rot = vec2(cos(layer), sin(layer));
    //density += dot(rot, TexCoords);

    // create pillars
    density += CreatePillars(rotPos, 3);

    // shelves
    //density += cos(layer);

    vec2 vec = vec2(cos(13 * layer/bufferHeight + yOffset), sin(13 * layer/bufferHeight + yOffset));
    density += dot(vec, TexCoords - vec2(0.5));

    // remove outer values
    density -= pow(length(TexCoords - vec2(0.5)), 1.5);
    //density -= pow(length(rotPos), 3.5);

    FragColor = vec4(density, 0, 0, 1);
}
