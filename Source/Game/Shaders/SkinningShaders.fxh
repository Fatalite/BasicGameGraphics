//--------------------------------------------------------------------------------------
// File: SkinningShaders.fx
//
// Copyright (c) Microsoft Corporation.
//--------------------------------------------------------------------------------------
#define NUM_LIGHTS (2)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
static const unsigned int MAX_NUM_BONES = 256u;


Texture2D txDiffuse : register(t0);

SamplerState samLinear : register(s0);


cbuffer cbSkinning : register(b4)
{
    matrix BoneTransforms[MAX_NUM_BONES];
};

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
    uint4 BoneIndices : BONEINDICES;
    float4 BoneWeights : BONEWEIGHTS;


};
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
};


PS_INPUT VSPhong(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    matrix skinTransform = (matrix) 0;
    skinTransform += BoneTransforms[input.BoneIndices.x] * input.BoneWeights.x;
    skinTransform += BoneTransforms[input.BoneIndices.y] * input.BoneWeights.y;
    skinTransform += BoneTransforms[input.BoneIndices.z] * input.BoneWeights.z;
    skinTransform += BoneTransforms[input.BoneIndices.w] * input.BoneWeights.w;
    
    output.Position = mul(input.Position, skinTransform);
    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.TexCoord = input.TexCoord;
    
    output.WorldPosition = normalize(mul(input.Position, World));
    output.Normal = normalize(mul(float4(input.Normal, 0), World).xyz);

    
    return output;
};

float4 PSPhong(PS_INPUT input) : SV_TARGET
{
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 ambience = float3(0.1f, 0.1f, 0.1f);
    float3 ambienceTerm = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 viewDirection = normalize(input.WorldPosition - CameraPosition.xyz);
    
    for (uint i = 0; i < NUM_LIGHTS; ++i)
    {
        ambienceTerm += (ambience * txDiffuse.Sample(samLinear, input.TexCoord).rgb) * LightColors[i].xyz;
        
        float3 lightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
        float lambertianTerm = dot(normalize(input.Normal), -lightDirection);
        diffuse += max(lambertianTerm, 0.0f) * txDiffuse.Sample(samLinear, input.TexCoord).rgb * LightColors[i].xyz;
        
        float3 reflectDirection = normalize(reflect(lightDirection, input.Normal));
        specular += pow(max(dot(-viewDirection, reflectDirection), 0.0f), 8.0f) * LightColors[i].xyz * txDiffuse.Sample(samLinear, input.TexCoord).rgb;
    }
    
	
    return float4(saturate(diffuse + specular + ambience), 1.0f);
    
}