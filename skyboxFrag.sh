#version 430
out vec4 FragColour;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 fogColour;

const float lowerLimit = 0.0;
const float upperLimit = 0.5;

void main()
{
	vec4 finalColour = texture(skybox, TexCoords);

	float factor = (TexCoords.y - lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor, 0.0, 1.0);

	FragColour = mix(vec4(fogColour, 1), finalColour, factor);
}