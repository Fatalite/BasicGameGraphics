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
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;

};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSVoxel(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Position = mul(input.Position, input.Transform);
    output.Position = mul(output.Position, World);
    output.WorldPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    //output.TexCoord = input.TexCoord;

    output.Normal = normalize(mul(float4(input.Normal, 0), World).xyz);
    
    output.Color = OutputColor;
    return output;

}

float4 PSVoxel(PS_INPUT input) : SV_Target
{
    float3 diffuse = float3(0.1f, 0.1f, 0.1f);
    float3 ambience = float3(0.1f, 0.1f, 0.1f);
    float3 ambienceTerm = float3(0.1f, 0.1f, 0.1f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 viewDirection = normalize(input.WorldPosition - CameraPosition.xyz);
    
    for (uint i = 0; i < NUM_LIGHTS; ++i)
    {
        ambienceTerm += (ambience) * LightColors[i].xyz;
        
        float3 lightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
        float lambertianTerm = dot(normalize(input.Normal), -lightDirection);
        diffuse += max(lambertianTerm, 0.0f) * LightColors[i].xyz;
        
        float3 reflectDirection = normalize(reflect(lightDirection, input.Normal));
        specular += pow(max(dot(-viewDirection, reflectDirection), 0.0f), 8.0f) * LightColors[i].xyz;
    }
    
	
    return float4(saturate(diffuse + specular + ambience), 1.0f) * input.Color;

}