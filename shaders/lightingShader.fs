#version 430 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

layout (binding = 1) uniform sampler2D diffuseTexture;
layout (binding = 0) uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float GetPCFShadows(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float LinearStep(float low, float high, float value)
{
    return clamp((value - low) / (high - low), 0.0, 1.0);
}

float GetVSMShadows(vec4 fragPosLightSpace)
{
    // perform perspective divide
    mediump vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // If we're outside of [0.0, 1.0] in the coordinates, return 0
    if (projCoords.x < 0.0 || projCoords.y < 0.0 || projCoords.x > 1.0 || projCoords.y > 1.0) return 0.0;

    // Variance Shadow Map Calculation
    vec2 moments = texture(shadowMap, projCoords.xy).rg;
    //moments.y = moment.x * moments.x; // simulate hard shadows

    // check whether current frag pos is in shadow
    float p = step(projCoords.z, moments.x);
    // We divide by this later, so make sure it's not exactly 0
    // It seems like it should always be 0.0, but due to interpolation it's not -- it increases with the deviation!
    float variance = max(moments.y - moments.x * moments.x, 0.00002);

    float d = (projCoords.z - moments.x) * 1.0;
    float p_max = variance / (variance + d * d);

    //float d = (projCoords.z - moments.x) * 10.0;
    //float p_max = LinearStep(0.2, 1.0, variance / (variance + d*d));

    // If this pixel is exactly in the light, p is 1, so make sure we return that in that case
    // min() to make sure that it doesn't get greater than 1.0
    return 1.0 - min(max(p, p_max), 1.0);
}

// TODO: CSM, ESM, PCSS, ...

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // calculate shadows
    //float shadow = GetPCFShadows(fs_in.FragPosLightSpace);
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    float shadow = GetVSMShadows(fs_in.FragPosLightSpace);
    vec3 lighting = ambient + color + specular + diffuse - (1 - shadow) * 0.25;
    //vec3 lighting = ambient + color + specular + diffuse - shadow * 0.25;

    FragColor = vec4(lighting, 1.0);
}