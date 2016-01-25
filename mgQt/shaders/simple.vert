#version 330 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 object_matrix;

in vec4 vtx_position;

void main()
{
	gl_Position = projection_matrix * modelview_matrix * object_matrix * vtx_position;
}
