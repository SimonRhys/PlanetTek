#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec3 Normal;
out vec2 Texcoord;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1);
	FragPos = vec3(model * vec4(position, 1));
	Normal = normal;
	Texcoord = texcoord;
}