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
cbuffer  cbChangesEveryFrame : register(b0)
{
	matrix World;
    float4 OutputColor;
};


struct VS_PHONG_INPUT
{
	float4 Position : POSITION;
    float2 TexCoord :TEXCOORD0;
    float3 Normal : NORMAL;


};
struct PS_PHONG_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
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
    
    for (uint i = 0; i < NUM_LIGHTS; ++i)  
    {     
        ambienceTerm += (ambience * txDiffuse.Sample(samLinear, input.TexCoord).rgb) * LightColors[i].xyz;  
        
        float3 lightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
		float lambertianTerm = dot(normalize(input.Normal), -lightDirection);
		diffuse += max(lambertianTerm, 0.0f) * txDiffuse.Sample(samLinear, input.TexCoord).rgb * LightColors[i].xyz;
        
		float3 reflectDirection = normalize(reflect(lightDirection, input.Normal));
		specular += pow(max(dot(-viewDirection, reflectDirection), 0.0f), 8.0f) * LightColors[i].xyz * txDiffuse.Sample(samLinear, input.TexCoord).rgb;
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
    
    return float4(diffuse.xyz,1)* float4(txDiffuse.Sample(
        samLinear,
        input.TexCoord));
}