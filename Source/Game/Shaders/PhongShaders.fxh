#ifndef NUM_LIGHTS
    #define NUM_LIGHTS 2
#endif

Texture2D txDiffuse[2] : register(t0);
/*--------------------------------------------------------------------
  TODO: Declare texture array and sampler state array for diffuse texture and normal texture (remove the comment)
--------------------------------------------------------------------*/
SamplerState samLinear[2] : register(s0);

cbuffer cbLights : register(b3)
{
 float4 LightPositions[2];
 float4 LightColors[2];
};

cbuffer cbChangeOnCameraMovement : register(b0)
{

	matrix View;
    float4 CameraPosition;

};

cbuffer CBChangeOnResize : register(b1)
{

	matrix Projection;
};
cbuffer  cbChangesEveryFrame : register(b2)
{
	matrix World;
    float4 OutputColor;
    bool HasNormalMap;
};


struct VS_PHONG_INPUT
{
	float4 Position : POSITION;
    float2 TexCoord :TEXCOORD0;
    float3 Normal : NORMAL;
    
        
    //Apply Tangent Data
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    

};
struct PS_PHONG_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
    
        
    //Apply Tangent Data
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    
};


struct PS_LIGHT_CUBE_INPUT
{
    float4 Position : SV_POSITION;
};

// CEL SHADER
struct PS_CEL_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition: WORLDPOS;
};

struct VS_CEL_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_CEL_INPUT VSCel(VS_CEL_INPUT input)
{
    PS_CEL_INPUT output = (PS_CEL_INPUT)0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.TexCoord = input.TexCoord;

    output.Normal = normalize(mul(float4(input.Normal,0),World).xyz);

    output.WorldPosition = normalize(mul( input.Position, World ));
    return output;
};


PS_PHONG_INPUT VSPhong(VS_PHONG_INPUT input)
{
    PS_PHONG_INPUT output = (PS_PHONG_INPUT )0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.TexCoord = input.TexCoord;
    
    if (HasNormalMap)
    {
        output.Tangent = normalize(mul(float4(input.Tangent, 0.0f), World).xyz);
        output.Bitangent = normalize(mul(float4(input.Bitangent, 0.0f), World).xyz);
    }
    
    output.Normal = normalize(mul(float4(input.Normal,0),World).xyz);

    output.WorldPosition = normalize(mul( input.Position, World ));
    return output;
};

PS_LIGHT_CUBE_INPUT VSLightCube(VS_PHONG_INPUT input)
{
    PS_LIGHT_CUBE_INPUT output = (PS_LIGHT_CUBE_INPUT)0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    

    return output;
} 

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSPhong(PS_PHONG_INPUT input) : SV_TARGET
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
        ambienceTerm += (ambience * txDiffuse[0].Sample(samLinear[0], input.TexCoord).rgb) * LightColors[i].xyz;  
        
        float3 lightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
		float lambertianTerm = dot(normalize(input.Normal), -lightDirection);
        diffuse += max(lambertianTerm, 0.0f) * txDiffuse[0].Sample(samLinear[0], input.TexCoord).rgb * LightColors[i].xyz;
        
		float3 reflectDirection = normalize(reflect(lightDirection, input.Normal));
		specular += pow(max(dot(-viewDirection, reflectDirection), 0.0f), 8.0f) * LightColors[i].xyz * txDiffuse[0].Sample(samLinear[0], input.TexCoord).rgb;
    }
    
	
    return float4(saturate( diffuse+specular+ambience), 1.0f);
    
}

float4 PSLightCube(PS_LIGHT_CUBE_INPUT input) : SV_Target
{
    return OutputColor;
}

float4 PSCel(PS_CEL_INPUT input) : SV_TARGET
{
    float3 diffuse = 0;
    for(uint i=0; i<2; ++i){
        float3 lightDir = normalize(input.WorldPosition - LightPositions[i].xyz);
        diffuse += saturate(dot(input.Normal,-(float3)lightDir)) * LightColors[i].xyz;
        diffuse = ceil(diffuse * 5) / 5.0f;
    }
    
    return float4(diffuse.xyz,1)* float4(txDiffuse[0].Sample(
        samLinear[0],
        input.TexCoord));
}