#version 330 core

// Colors
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;

in vec3 fragNorm;
in vec3 fragLight;
out vec3 color;

void main()
{
    vec3 N = normalize(fragNorm);
    vec3 L = normalize(fragLight);

    float l_dot_n = clamp(dot(L, N), 0.0, 1.0);
    color =  MatDif * l_dot_n + MatSpec * pow(l_dot_n, shine) + MatAmb;
}