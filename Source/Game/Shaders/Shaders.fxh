Texture2D txDiffuse : register(t0);

SamplerState samLinear : register(s0);
cbuffer cbView : register(b2)
{

	matrix View;

};

cbuffer cbProjection : register(b1)
{

	matrix Projection;
};
cbuffer  cbWorld  : register(b0)
{
	matrix World;

};


/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_INPUT
  Summary:  Used as the input to the vertex shader 
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/

struct VS_INPUT
{
	float4 Pos : POSITION;
    float2 TexCoord :TEXCOORD0;

};
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_INPUT
  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord :TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.TexCoord = input.TexCoord;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample(
        samLinear,
        input.TexCoord);
}