#version 330 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 object_matrix;

in vec3 vtx_position;
in vec3 vtx_color;

out vec3 vertexV;
out vec3 colorV;

void main()
{
	vec4 view_vtx = modelview_matrix * object_matrix * vec4(vtx_position, 1.);
	colorV = vtx_color;
	gl_Position = projection_matrix * view_vtx;
}
