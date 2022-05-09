#ifndef NUM_LIGHTS
#define NUM_LIGHTS 2
#endif

Texture2D txDiffuse : register(t0);

SamplerState samLinear : register(s0);

cbuffer cbLights : register(b3)
{
    float4 LightPositions[2];
    float4 LightColors[2];
};

cbuffer cbChangeOnCameraMovement : register(b2)
{

    matrix View;
    float4 CameraPosition;

};

cbuffer CBChangeOnResize : register(b1)
{

    matrix Projection;
};
cbuffer cbChangesEveryFrame : register(b0)
{
    matrix World;
    float4 OutputColor;
};



struct VS_INPUT
{
	float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
	row_major matrix Transform : INSTANCE_TRANSFORM;


};
struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR;

};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSVoxel(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;
	//transform(matrix)
	/*
	output.Position.x = output.Position.x + input.Transform[0][3];
	output.Position.y = output.Position.y + input.Transform[1][3];
	output.Position.z = output.Position.z + input.Transform[2][3];
	*/

	output.Position = mul(output.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	
    output.Color = OutputColor;
	return output;

}

float4 PSVoxel(PS_INPUT input) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
	//return float4(input.Color, 1.0f);

}
