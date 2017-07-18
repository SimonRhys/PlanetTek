#version 430
in vec3 FragPos;
in vec3 Normal;
in vec3 Texcoord;

out vec4 colour;

uniform sampler2D texSampler0;
uniform sampler2D texSampler1;
uniform vec3 lightPos; 
uniform vec3 lightColor;

void main()
{
	float minHeight = 1024*1000*1.2;
	float maxHeight = 1024*1000*1.35;
	float scaleFactor = (length(FragPos) - minHeight) / (maxHeight - minHeight);
	scaleFactor = min(scaleFactor, 1);
	scaleFactor = max(scaleFactor, 0);

	vec4 texColor = mix(texture(texSampler0, Texcoord.xy), texture(texSampler1, Texcoord.xy), scaleFactor);

	vec3 objectColor = vec3(1, 1, 1);

	if(Texcoord.x < 0 || Texcoord.y < 0)
	{
		objectColor = vec3(1, 1, 1);
	}
	else
	{
		objectColor = texColor.xyz;
	}

    // Ambient
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    colour = vec4(result, 1.0f);	
}