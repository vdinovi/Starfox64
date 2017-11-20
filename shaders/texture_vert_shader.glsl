#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float Shine;
uniform vec2 texOffset;

out vec3 fragNor;
out vec3 fragPos;
out vec2 fragTexCoord;
out vec3 fragLight;

void main()

{
	gl_Position = P * V * M * vertPos;

	fragPos = gl_Position.xyz;
	fragNor = (transpose(inverse(M)) * vec4(vertNor, 0.0)).xyz;
    fragLight = normalize(lightPos - gl_Position.xyz);
	fragTexCoord = vertTex + texOffset;
}

