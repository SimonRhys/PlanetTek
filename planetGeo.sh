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
	vec3 visibility;
} gs_out;

out vec4 ClipSpaceCoords;
out vec3 toCameraVector;


uniform mat4 projection;
uniform mat4 view;
uniform vec3 eyePos;
uniform float seaLevel;
uniform int renderSea;
uniform int reflection;
uniform int refraction;

const float density = 0.00001;
const float gradient = 5.0;

vec3 calculateVisibility(vec4 p)
{
	vec4 posRelativeToCamera = view * p;
	float distance = length(posRelativeToCamera);
	float visibility = exp(-pow(distance*density, gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	return vec3(visibility, 0, 0);
}

vec3 createOceanTriangle(vec3 p)
{
	vec3 dir = normalize(p);
	vec3 newP = dir * seaLevel;
	
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

void setClipDistance(vec3 p)
{
	if(reflection != 0)
	{
		gl_ClipDistance[0] = length(p) - length(seaLevel);
	}
	else if(refraction != 0)
	{
		gl_ClipDistance[0] = length(seaLevel) - length(p);
	}
}

void main() 
{    
	gs_out.ocean = vec3(0, 0, 0);

	gs_out.fragPos = gs_in[0].fragPos;
	gs_out.normal = gs_in[0].normal;
	gs_out.texcoord = gs_in[0].texcoord;
	gs_out.visibility = calculateVisibility(vec4(gs_in[0].fragPos, 1));
    gl_Position = gl_in[0].gl_Position;
	ClipSpaceCoords = gl_in[0].gl_Position;

	setClipDistance(gs_in[0].fragPos);
	toCameraVector = eyePos - gs_in[0].fragPos;

    EmitVertex();

	gs_out.fragPos = gs_in[1].fragPos;
	gs_out.normal = gs_in[1].normal;
	gs_out.texcoord = gs_in[1].texcoord;
	gs_out.visibility = calculateVisibility(vec4(gs_in[1].fragPos, 1));
    gl_Position = gl_in[1].gl_Position;
	ClipSpaceCoords = gl_in[1].gl_Position;

	setClipDistance(gs_in[1].fragPos);
	toCameraVector = eyePos - gs_in[1].fragPos;

    EmitVertex();

	gs_out.fragPos = gs_in[2].fragPos;
	gs_out.normal = gs_in[2].normal;
	gs_out.texcoord = gs_in[2].texcoord;
	gs_out.visibility = calculateVisibility(vec4(gs_in[2].fragPos, 1));
    gl_Position = gl_in[2].gl_Position;
	ClipSpaceCoords = gl_in[2].gl_Position;

	setClipDistance(gs_in[2].fragPos);
	toCameraVector = eyePos - gs_in[2].fragPos;

    EmitVertex();
    
    EndPrimitive();

	if(aboveOcean() || renderSea == 0)
	{
		return;
	}
	
	gs_out.ocean = vec3(1, 0, 0);

	vec4 viewPos;

	gs_out.fragPos = createOceanTriangle(gs_in[0].fragPos);
	gs_out.normal = normalize(gs_out.fragPos);
	gs_out.texcoord = gs_in[0].texcoord;
	viewPos = view * vec4(gs_out.fragPos, 1);
	gs_out.visibility = calculateVisibility(viewPos);
	ClipSpaceCoords =  projection * viewPos;
	gl_Position = ClipSpaceCoords;
	setClipDistance(gs_out.fragPos);
	toCameraVector = eyePos - gs_out.fragPos;

    EmitVertex();

	gs_out.fragPos = createOceanTriangle(gs_in[1].fragPos);
	gs_out.normal = normalize(gs_out.fragPos);
	gs_out.texcoord = gs_in[1].texcoord;
	viewPos = view * vec4(gs_out.fragPos, 1);
	gs_out.visibility = calculateVisibility(viewPos);
	ClipSpaceCoords =  projection * viewPos;
	gl_Position = ClipSpaceCoords;
	setClipDistance(gs_out.fragPos);
	toCameraVector = eyePos - gs_out.fragPos;

    EmitVertex();

	gs_out.fragPos = createOceanTriangle(gs_in[2].fragPos);
	gs_out.normal = normalize(gs_out.fragPos);
	gs_out.texcoord = gs_in[2].texcoord;
	viewPos = view * vec4(gs_out.fragPos, 1);
	gs_out.visibility = calculateVisibility(viewPos);
	ClipSpaceCoords =  projection * viewPos;
	gl_Position = ClipSpaceCoords;
	setClipDistance(gs_out.fragPos);
	toCameraVector = eyePos - gs_out.fragPos;

    EmitVertex();
    
    EndPrimitive();
}  