TextureCube g_txSky : register(t0);
SamplerState g_Sample : register(s0);

struct VS_INPUT
{
    float4 Position : POSITION;
    float3 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;

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
    bool HasNormalMap;
};


VS_OUTPUT VSCubeMap(VS_INPUT vIn)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(vIn.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.TexCoord = vIn.Position;
    
    output.Normal = normalize(mul(float4(vIn.Normal, 0), World).xyz);
    return output;
}


float4 PSCubeMap(VS_OUTPUT input) : SV_TARGET
{
	//텍스처에서 t좌표에 해당하는 컬러값(픽셀) 반환
    return g_txSky.Sample(g_Sample, input.TexCoord);
}