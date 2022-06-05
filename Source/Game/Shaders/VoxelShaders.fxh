#ifndef NUM_LIGHTS
#define NUM_LIGHTS 2
#endif

Texture2D txDiffuse[2] : register(t0);
SamplerState samLinear[2] : register(s0);

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
cbuffer  cbChangesEveryFrame : register(b0)
{
	matrix World;
    float4 OutputColor;
    bool HasNormalMap;
};




struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    
    //Apply Tangent Data
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    
    row_major matrix Transform : INSTANCE_TRANSFORM;


};
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
    
        //Apply Tangent Data
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    

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
    
    
    output.TexCoord = input.TexCoord;
    output.Normal = mul(float4(input.Normal, 0.0f), input.Transform).xyz;
    output.Normal = mul(float4(output.Normal, 0.0f), World).xyz;
    if (HasNormalMap)
    {
        output.Tangent = normalize(mul(float4(input.Tangent, 0.0f), World).xyz);
        output.Bitangent = normalize(mul(float4(input.Bitangent, 0.0f), World).xyz);
    }
    //output.Color = OutputColor;
    return output;

}

float4 PSVoxel(PS_INPUT input) : SV_Target
{
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 ambience = float3(0.1f, 0.1f, 0.1f);
    float3 ambienceTerm = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 viewDirection = normalize(input.WorldPosition - CameraPosition.xyz);
    float3 normal = normalize(input.Normal);
    
    if (HasNormalMap)
    {
        // Sample the pixel in the normal map.
        float4 bumpMap = txDiffuse[1].Sample(samLinear[1], input.TexCoord);
        // Expand the range of the normal value from (0, +1) to (-1, +1).
        bumpMap = (bumpMap * 2.0f) - 1.0f;
        // Calculate the normal from the data in the normal map.
        float3 bumpNormal = (bumpMap.x * input.Tangent) + (bumpMap.y * input.Bitangent) +
(bumpMap.z * normal);
        // Normalize the resulting bump normal and replace existing normal
        normal = normalize(bumpNormal);
    }
    
    
    for (uint i = 0; i < NUM_LIGHTS; ++i)
    {
        ambienceTerm += (ambience) * LightColors[i].xyz;
        
        float3 lightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
        float lambertianTerm = dot(normalize(normal), -lightDirection);
        diffuse += max(lambertianTerm, 0.0f) *  LightColors[i].xyz;
        
        float3 reflectDirection = normalize(reflect(lightDirection, normal));
        specular += pow(max(dot(-viewDirection, reflectDirection), 0.0f), 20.0f) * LightColors[i].xyz;
    }
    
	
    return float4(saturate(diffuse + specular + ambience), 1.0f)* float4(txDiffuse[0].Sample(
        samLinear[0],
        input.TexCoord));

}