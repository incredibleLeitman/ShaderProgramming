#version 430

in vec3 varPosition;

out float noise;

void main(void) {
    // Just to random stuff to the numbers until it looks nice
    float f1 = sin((varPosition.x + varPosition.z) * 10.0);
    float f2 = cos((varPosition.y + varPosition.x) * 7.0);
    float f3 = cos((varPosition.z + varPosition.y) * 8.0);
    
    noise = (f1 * f1 + f2 * f2 + f3 * f3) * (1.0 / 3.0);
}
