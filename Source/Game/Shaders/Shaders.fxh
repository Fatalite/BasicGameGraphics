
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  ConstantBuffer
  Summary:  Constant buffer used for space transformations
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer ConstantBuffer : register(b0)
{ 
    matrix World;
    matrix View;
    matrix Projection;
}
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_INPUT
  Summary:  Used as the input to the vertex shader 
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
struct VS_INPUT{
    float3 Pos : POSITION;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_INPUT
  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/

struct VS_OUTPUT{
    float4 Pos : SV_POSITION;
    float3 Color : COLOR;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT vin)
{
VS_OUTPUT output = (VS_OUTPUT)0;
output.Pos = mul(vin.Pos, World);
output.Pos = mul(output.Pos, View);
output.Pos = mul(output.Pos, Projection);

return output;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT pin): SV_Target{
    
    return (1.0f,0.0f,0.0f,1.0f);
};