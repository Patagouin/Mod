#version 330 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 object_matrix;

in vec3 vtx_position;
in vec3 vtx_normal;

void main()
{
	gl_Position = projection_matrix * modelview_matrix * object_matrix * vec4(vtx_position, 1.);
}
