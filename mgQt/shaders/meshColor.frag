#version 330 core


in vec3 colorV;

out vec4 out_color;


void main()
{
	out_color = vec4(colorV, 1.0);
}
