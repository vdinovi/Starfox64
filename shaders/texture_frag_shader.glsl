#version 330 core
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float Shine;

in vec3 fragPos;
in vec3 fragNor;
in vec3 fragLight;
in vec2 fragTexCoord;

out vec3 color;

uniform sampler2D uTexture;

void main()
{
        vec3 N = normalize(fragNor);
        vec3 L = normalize(fragLight);

        float l_dot_n = clamp(dot(L, N), 0.0, 1.0);
        //color =  MatAmb *  + MatDif * l_dot_n * texture(uTexture, fragTexCoord).xyz + MatSpec * pow(l_dot_n, shine);
        color = texture(uTexture, fragTexCoord).xyz;
}
