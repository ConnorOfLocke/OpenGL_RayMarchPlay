#version 410

out vec4 final_colour;

uniform mat4 view_matrix;

void main()
{
	gl_Position = position;
}
