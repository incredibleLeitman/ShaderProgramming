//#version 330 core
#version 430 // version 430 allows array with initializer list

uniform int layer;
uniform float yOffset;
uniform float bufferHeight;

in vec2 TexCoords;
//in vec3 fPos;

out vec4 FragColor;

/*vec2 pillars[3] =
{
    vec2(0.2,0.6), vec2(0.7,0.8), vec2(0.6,0.3)
};*/
vec2 pillars[2] =
{
    vec2(0.2,0.6), vec2(0.7,0.8)
};

void main()
{
    float thisVal = 0;
    for (int i = 0; i < 2; i++)
    {
        thisVal += 0.05/length(TexCoords - pillars[i]) - 0.05;
        //thisVal += 1/length(fPos.xz - pillars[i]) - 1;
    }
    vec2 vec = vec2(cos(13 * layer/bufferHeight + yOffset), sin(13 * layer/bufferHeight + yOffset));
    thisVal += dot(vec, TexCoords - vec2(0.5));
    //thisVal += 0.1 * random(length(TexCoords - vec2(0.5)));
    //thisVal = thisVal > 1 ? 1 : thisVal;
    thisVal -= pow(length(TexCoords - vec2(0.5)),1.5);

    //thisVal = thisVal > 0.5 ? 1 : 0;
    //FragColor = vec4(thisVal, 0, 0, 1);
    FragColor = vec4(thisVal, thisVal, thisVal, 1);
}
