#version 430 core
layout (location = 0) in vec3 aPos;

uniform float time;
uniform float seed;
uniform vec3 maxVelocity;
uniform vec3 minVelocity;
uniform float minLifeTime;
uniform float maxLifeTime;
uniform int maxParticles;
uniform int particlesPerSecond;

out vec4 gColor;
out float gAlive;
out float gSize;

// able to generate 3 different types of particle
// regarding color and size
vec4 colors[] = vec4[](
    vec4(1.0, 0.0, 0.0, 0.5),
    vec4(0.0, 1.0, 0.0, 0.75),
    vec4(0.0, 0.0, 1.0, 1.0)
);
float sizes[] = float[](1, 0.5, 0.25);

// create a random number between 0 and 1
// https://thebookofshaders.com/10/
float random (vec2 v2)
{
    return fract(sin(dot(v2*seed, vec2(12.9898, 78.233))) * 43758.5453123);
}

float randomRange (float min, float max)
{
    return min + random(vec2(gl_VertexID, gl_VertexID)) * (max - min);
}

vec3 calculateParticleVelocity ()
{
    float xRand = random(vec2(1, gl_VertexID));
    float yRand = random(vec2(2, gl_VertexID));
    float zRand = random(vec2(3, gl_VertexID));
    vec3 velocity = vec3(
        minVelocity.x * xRand + maxVelocity.x * (1 - xRand),
        minVelocity.y * yRand + maxVelocity.y * (1 - yRand),
        minVelocity.z * zRand + maxVelocity.z * (1 - zRand));
    return velocity;
}

void main ()
{
    float lifeTime = randomRange(minLifeTime, maxLifeTime);

    // time from the moment of the particle spawn
    float offsetTime = max(0, time - gl_VertexID / float(particlesPerSecond));

    // offsetTime should loop around after a fool loop of the system
    // if lifetime is bigger than loop amount use this as loop basis
    offsetTime = mod(offsetTime, max(maxParticles / particlesPerSecond, lifeTime));
    gAlive = float(offsetTime < lifeTime && offsetTime != 0);

    vec3 velocity = calculateParticleVelocity();
    vec3 pos = velocity * offsetTime;

    int typeIndex = int(randomRange(0, 3));

    // normalized point in the current lifetime of the particle to use for different lifetime depended effects
    float timeAliveFactor = (lifeTime - offsetTime) / lifeTime;

    // color fades out over lifetime
    gColor = colors[typeIndex];
    gColor.a = gColor.a * timeAliveFactor;

    // size decreases over lifetime
    gSize = sizes[typeIndex] * timeAliveFactor;

    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}