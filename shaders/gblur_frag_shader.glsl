#version 330 core

in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D uTexture;
uniform float offset[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );

void main(){
    color = vec4(0, 0, 0, 0);

    for (int j = 1; j <= 5; j++) {
        vec4 rowColor = vec4(texture( uTexture, fragTexCoord - vec2(0.0, offset[j])).rgb, 1)*weight[0];
        for (int i = 1; i <= 5; i++) {
            rowColor += vec4(texture(uTexture, fragTexCoord + vec2(offset[i], -offset[j])/512.0 ).rgb, 1)*weight[i];
            rowColor += vec4(texture(uTexture, fragTexCoord - vec2(offset[i], offset[j])/512.0 ).rgb, 1)*weight[i];
        }
        color += rowColor*weight[j];
    }
    for (int j = 1; j <= 5; j++) {
        vec4 rowColor = vec4(texture( uTexture, fragTexCoord + vec2(0.0, offset[j])).rgb, 1)*weight[0];
        for (int i = 1; i <= 5; i++) {
            rowColor += vec4(texture(uTexture, fragTexCoord + vec2(offset[i], offset[j])/512.0 ).rgb, 1)*weight[i];
            rowColor += vec4(texture(uTexture, fragTexCoord - vec2(offset[i], -offset[j])/512.0 ).rgb, 1)*weight[i];
        }
        color += rowColor*weight[j];
    }
    vec4 rowColor = vec4(texture(uTexture, fragTexCoord - vec2(0.0, 0.0)).rgb, 1)*weight[0];
    for (int i = 1; i <= 5; i++) {
        rowColor += vec4(texture(uTexture, fragTexCoord + vec2(offset[i], 0.0)/512.0 ).rgb, 1)*weight[i];
        rowColor += vec4(texture(uTexture, fragTexCoord - vec2(offset[i], 0.0)/512.0 ).rgb, 1)*weight[i];
    }
    color += rowColor*weight[0];
}