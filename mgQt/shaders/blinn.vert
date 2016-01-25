#version 330 core

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 object_matrix;
uniform mat3 normal_matrix;

in vec3 vtx_position;
in vec3 vtx_normal;

out vec3 vertexV;
out vec3 normalV;

void main()
{
	vec4 view_vtx = modelview_matrix * object_matrix * vec4(vtx_position, 1.);
	vertexV = view_vtx.xyz;
	normalV = normal_matrix * vtx_normal;

	gl_Position = projection_matrix * view_vtx;
}
