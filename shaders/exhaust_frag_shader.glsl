#version 330 core

//uniform float flicker;

in vec3 fragPos;

out vec4 color;

void main()
{
        vec2 point = 2.0 * gl_PointCoord - 1.0;
        float r = dot(point, point);
	if (r > 0.6) {
		discard;
	} else if (r > 0.05) {
	        color = vec4((1-r), 0.4*(1-r), 0.05*(1-r), 0.5);
        } else {
	        color = vec4(1, 1, 1, 0.9);
        }
}