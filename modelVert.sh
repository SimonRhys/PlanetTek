#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	FragPos = vec3(model * vec4(position, 1));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords;

	gl_Position = projection * view * vec4(FragPos, 1.0);
}