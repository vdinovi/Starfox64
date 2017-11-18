#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

// Light
uniform vec3 lightDir;

out vec3 fragLight;
out vec3 fragNorm;

void main()

{
	gl_Position = P * V * M * vertPos;
    mat4 normM = inverse(transpose(M));
	fragNorm = normalize(normM * vec4(vertNor, 0)).xyz;
    fragLight = normalize(lightDir);
}