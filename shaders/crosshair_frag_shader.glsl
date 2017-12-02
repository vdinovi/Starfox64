#version 330 core
uniform sampler2D uTexture;

in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTexCoord;
out vec4 color;

void main()
{
        vec4 col = texture(uTexture, fragTexCoord);
        if (col.r > 0.8) {
                discard;
        }
        else {
                color = vec4(0.42, 0.93, 0.24, 0.6);
        }
}
