#version 330 core

in vec3 fragColor;
in vec2 fragTexCoord;

out vec3 color;

uniform sampler2D uTexture;

void main()
{
        color = texture(uTexture, fragTexCoord).xyz;
        //color = fragColor;
}
