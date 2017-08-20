#version 430
layout (location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoords = position;
	vec4 pos = projection * view * vec4(position, 1);
	gl_Position = pos.xyww;
}