#version 430
out vec4 FragColour;

in vec3 TexCoords;

uniform samplerCube skyboxDay;
uniform samplerCube skyboxNight;
uniform vec3 fogColour;
uniform int dayTime;
uniform float blendFactor;

const float lowerLimit = 0.0;
const float upperLimit = 0.5;

void main()
{
	vec4 finalColour;
	if(dayTime == 1)
	{
		finalColour = texture(skyboxDay, TexCoords);
	}
	else
	{
		finalColour = mix(texture(skyboxDay, TexCoords), texture(skyboxNight, TexCoords), blendFactor);
	}

	float factor = (TexCoords.y - lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor, 0.0, 1.0);

	FragColour = mix(vec4(fogColour, 1), finalColour, factor);
}