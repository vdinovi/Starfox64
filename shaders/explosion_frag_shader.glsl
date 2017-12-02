#version 330 core

in vec3 fragPos;

out vec4 color;


void main()
{
        vec2 point = 2.0 * gl_PointCoord - 1.0;
        float r = dot(point, point);
	if (r > 0.2) {
		discard;
        } else {
	        color = vec4(0.4, 0.2, 0.0, 0.8);
        }

}

