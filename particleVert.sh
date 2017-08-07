#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 offset;
layout (location = 2) in vec3 colour;

//uniform mat4 projection;
//uniform mat4 view;
uniform mat4 model;

out VS_OUT {
	vec3 colour;
} vs_out;



void main()
{
	gl_Position = model * vec4(position+offset, 1);
	vs_out.colour = colour;
}