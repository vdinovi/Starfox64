#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

out vec3 fragColor;
out vec2 fragTexCoord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec2 texOffset;

void main()
{
	fragTexCoord = vertTex;
}

