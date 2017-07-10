#version 430
in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;

out vec4 colour;

uniform sampler2D texSampler;
uniform vec3 lightPos; 
uniform vec3 lightColor;

void main()
{

	vec4 texColor = texture(texSampler, Texcoord);
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
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    colour = vec4(result, 1.0f);	
}