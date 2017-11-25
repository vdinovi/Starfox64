#version 330 core

in vec3 fragColor;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D uTexture;

void main()
{
        color = texture(uTexture, fragTexCoord);
        /*color.r *= fragColor.r;
        color.g *= fragColor.g;
        color.b *= fragColor.b;
        color.a = 1.0;*/

}

