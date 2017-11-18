#version 330 core

in vec3 fragPos;
in vec3 fragNor;
out vec3 color;

void main()
{
        color = fragNor;
}
