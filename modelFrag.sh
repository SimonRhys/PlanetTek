#version 430
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 colour;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular2;
uniform sampler2D texture_normal2;
uniform sampler2D texture_height2;

uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular3;
uniform sampler2D texture_normal3;
uniform sampler2D texture_height3;

void main()
{
	colour = texture(texture_diffuse1, TexCoords);
}