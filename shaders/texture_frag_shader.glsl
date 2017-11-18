#version 330 core

in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D uTexture;

void main()
{
        color = texture(uTexture, fragTexCoord);
        //color = vec4(1.0, 1.0, 1.0, 0.0);
}
