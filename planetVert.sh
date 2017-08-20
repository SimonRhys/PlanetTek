#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float seaLevel;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec3 texcoord;
} vs_out;

void main()
{
	vec4 worldPosition = model * vec4(position, 1);

	gl_Position = projection * view * worldPosition;
	vs_out.fragPos = vec3(worldPosition);
	vs_out.normal = normal;
	vs_out.texcoord = texcoord;
}