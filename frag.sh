#version 430
in vec2 TexCoords;

out vec4 colour;

uniform sampler2D texSampler;

void main()
{
	if(TexCoords.x >= 0 && TexCoords.y >= 0) 
	{
		colour = texture(texSampler, TexCoords);
	} else
	{
		colour = vec4(1, 1, 1, 1);
	}
}