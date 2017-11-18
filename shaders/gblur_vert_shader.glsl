#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTexCoord;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 fragNor;
out vec3 fragPos;
out vec2 fragTexCoord;

void main()

{
	fragPos = vertPos.xyz;
	gl_Position = P * V * M * vertPos;
	fragNor = (transpose(inverse(M)) * vec4(vertNor, 0.0)).xyz;
	fragTexCoord = vertTexCoord;
}

