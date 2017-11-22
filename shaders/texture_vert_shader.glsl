#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

out vec3 fragColor;
out vec2 fragTexCoord;

// LightInfo
uniform vec3 lightPosition;
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;

// MaterialInfo
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Shine;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec2 texOffset;

vec3 lightFunc(vec3 position, vec3 norm)
{
	vec3 l = normalize(vec3(lightPosition - position));
	vec3 v = normalize(-position.xyz);
	vec3 r = reflect(-l, norm);

	//vec3 ambient = La * Ka;

	float lDotN = max(dot(l, norm), 0.0);
	vec3 diffuse = Ld * Kd * lDotN;

	//vec3 spec = Ls * Ks * pow(max(dot(r, v), 0.0), Shine);

	//return ambient + diffuse + spec;
	return diffuse;

}

void main()
{
	mat4 normMat = transpose(inverse(M));
	vec3 eyeNorm = normalize(normMat * vec4(vertNor, 0)).xyz;
	vec3 eyePosition = vec3(V * M * vertPos);

	fragColor = lightFunc(eyePosition, eyeNorm);
	fragTexCoord = vertTex + texOffset;
	gl_Position = P * V * M * vertPos;
}

