#version 430
in VS_OUT {
	vec3 colour;
} vs_out;

out vec4 colour;

void main()
{
    colour = vec4(vs_out.colour, 1.0f);	
}