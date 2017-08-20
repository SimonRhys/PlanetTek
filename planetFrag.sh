#version 430
in GS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec3 texcoord;
	vec3 ocean;
	vec3 visibility;
} gs_out;

in vec4 ClipSpaceCoords;
in vec3 toCameraVector;

out vec4 colour;

uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D waterDUDV;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform float seaLevel;
uniform float waveSpeed;
uniform vec3 eyePos;
uniform vec3 skyColour;

vec3 sunDirection = normalize(lightPos - gs_out.fragPos); //vec3(0.70707, 0.70707, 0);
vec3 sunColor = vec3(1.0);
vec3 waterColor = vec3(0.3, 0.5, 0.9); //vec3(0.496);

vec4 calculateWaterReflection()
{
	float waveStrength = 0.02;

	vec2 ndc = (ClipSpaceCoords.xy/ClipSpaceCoords.w) / 2 + 0.5;

	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);

	vec2 distortion1 = texture(waterDUDV, vec2(gs_out.texcoord.x + waveSpeed,gs_out.texcoord.y)).rg * 2.0 - 1.0;
	vec2 distortion2 = texture(waterDUDV, vec2(-gs_out.texcoord.x + waveSpeed,gs_out.texcoord.y + waveSpeed)).rg * 2.0 - 1.0;
	distortion1 = distortion1 * waveStrength;
	distortion2 = distortion2 * waveStrength;

	vec2 totalDistortion = distortion1 + distortion2;

	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 reflectionColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColour = texture(refractionTexture, refractTexCoords);

	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, gs_out.normal);
	//refractiveFactor = pow(refractiveFactor, 10.0);

	vec4 finalColour = mix(reflectionColour, refractionColour, refractiveFactor);
	//finalColour = mix(finalColour, vec4(0, 0.3, 0.5, 1.0), 0.2);

	return finalColour;
}

void main()
{

	if(gs_out.ocean.x == 1)
	{
	    colour = calculateWaterReflection(); //vec4(0.0, 0.2, 0.4, 1.0);
	}
	else 
	{
		float minHeight = seaLevel*1.1;
		float maxHeight = seaLevel*1.2;
		float scaleFactor = (length(gs_out.fragPos) - minHeight) / (maxHeight - minHeight);
		scaleFactor = min(scaleFactor, 1);
		scaleFactor = max(scaleFactor, 0);

		vec4 texColor = mix(texture(grassTexture, gs_out.texcoord.xy), texture(rockTexture, gs_out.texcoord.xy), scaleFactor);

		// Ambient
		float ambientStrength = 0.05f;
		vec3 ambient = ambientStrength * texColor.xyz;

		// Diffuse 
		vec3 norm = normalize(gs_out.normal);
		vec3 lightDir = normalize(lightPos - gs_out.fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * texColor.xyz;



		colour = vec4(ambient + diffuse, 1.0f);	
		colour = mix(vec4(skyColour, 1.0), colour, gs_out.visibility.x);
	}
}