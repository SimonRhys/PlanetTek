#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec3 texcoord;
} gs_in[];

out GS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec3 texcoord;
	vec3 ocean;
} gs_out;

uniform mat4 projection;
uniform mat4 view;
uniform float seaLevel;
uniform float time;

vec4 createOceanTriangle(vec3 p)
{
	vec3 dir = normalize(p);
	vec3 height = dir * seaLevel;
	height = height + dir * sin(time) * seaLevel * 0.001;
	vec4 newP = vec4(height, 1);

	newP = projection * view * newP;
	
	return newP;
}

bool aboveOcean()
{
	if(length(gs_in[0].fragPos) > seaLevel && 
		length(gs_in[1].fragPos) > seaLevel &&
		length(gs_in[2].fragPos) > seaLevel)
	{
			return true;
	}
	
	return false;
}

void main() 
{    
	gs_out.ocean = vec3(0, 0, 0);

	gs_out.fragPos = gs_in[0].fragPos;
	gs_out.normal = gs_in[0].normal;
	gs_out.texcoord = gs_in[0].texcoord;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

	gs_out.fragPos = gs_in[1].fragPos;
	gs_out.normal = gs_in[1].normal;
	gs_out.texcoord = gs_in[1].texcoord;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

	gs_out.fragPos = gs_in[2].fragPos;
	gs_out.normal = gs_in[2].normal;
	gs_out.texcoord = gs_in[2].texcoord;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();

	if(aboveOcean())
	{
		return;
	}
	
	gs_out.ocean = vec3(1, 0, 0);

	gs_out.fragPos = gs_in[0].fragPos;
	gs_out.normal = gs_in[0].normal;
	gs_out.texcoord = gs_in[0].texcoord;
    gl_Position = createOceanTriangle(gs_in[0].fragPos);
    EmitVertex();

	gs_out.fragPos = gs_in[1].fragPos;
	gs_out.normal = gs_in[1].normal;
	gs_out.texcoord = gs_in[1].texcoord;
    gl_Position = createOceanTriangle(gs_in[1].fragPos);
    EmitVertex();

	gs_out.fragPos = gs_in[2].fragPos;
	gs_out.normal = gs_in[2].normal;
	gs_out.texcoord = gs_in[2].texcoord;
    gl_Position = createOceanTriangle(gs_in[2].fragPos);
    EmitVertex();
    
    EndPrimitive();
}  