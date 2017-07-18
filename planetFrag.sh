#version 430
in GS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec3 texcoord;
} gs_out;

out vec4 colour;

uniform sampler2D texSampler0;
uniform sampler2D texSampler1;
uniform vec3 lightPos; 
uniform vec3 lightColor;

void main()
{
	float minHeight = 1024*1000*1.2;
	float maxHeight = 1024*1000*1.35;
	float scaleFactor = (length(gs_out.fragPos) - minHeight) / (maxHeight - minHeight);
	scaleFactor = min(scaleFactor, 1);
	scaleFactor = max(scaleFactor, 0);

	vec4 texColor = mix(texture(texSampler0, gs_out.texcoord.xy), texture(texSampler1, gs_out.texcoord.xy), scaleFactor);

	vec3 objectColor = vec3(1, 1, 1);

	if(gs_out.texcoord.x < 0 || gs_out.texcoord.y < 0)
	{
		objectColor = vec3(0, 0, 1);
	}
	else
	{
		objectColor = texColor.xyz;
	}

    // Ambient
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 
    vec3 norm = normalize(gs_out.normal);
    vec3 lightDir = normalize(lightPos - gs_out.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    colour = vec4(result, 1.0f);	
}