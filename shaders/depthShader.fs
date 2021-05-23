#version 430 core

void main()
{
    //gl_FragDepth = gl_FragCoord.z; // not needed ->OpenGL does it anyway

    // calculate variance with mean and mean squared
    gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 0.0);
}