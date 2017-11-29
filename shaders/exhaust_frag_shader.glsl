#version 330 core

uniform bool Flicker;

in vec3 fragPos;

out vec4 color;

void main()
{
        vec2 point = 2.0 * gl_PointCoord - 1.0;
        float r = dot(point, point);
	//if (Flicker ? r > 0.6 : r > 0.4) {
	if (r > 0.6) {
		discard;
	} else if (r > 0.05) {
	        color = vec4(1, 0, 0, 0.5);
        } else {
	        color = vec4(1, 1, 1, 0.9);
        }
}