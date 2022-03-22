#pragma once
#include <common.h>
//-----//
#include <windowsnumerics.h>
using namespace Windows::Foundation::Numerics;

struct Vertex1 {
	XMFLOAT3 Pos;
	XMFLOAT4 Color;

};

struct Vertex2 {
	XMFLOAT3 Pos; //offset 0
	XMFLOAT3 Normal; //offset 12
	XMFLOAT2 Tex0; //offset 24
	XMFLOAT2 Tex1; //offset 32
}; // 우리가 정의


/*
typedef struct D3D11_INPUT_LAYOUT_DESC
{
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs;
	UINT NumElements;

}; D3D11_INPUT_LAYOUT_DESC;
*/

D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
	D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,
	D3D11_INPUT_PER_VERTEX_DATA, 0},

};

struct VertexOut {
	//float4 position : POSITION;
	//float4 color : COLOR;
};

/*
Each element in the vertex structure is described by a corresponding element in the D3D11_INPUT_ELEMENT_DESC array. 
The semantic name and index provides a way 
for mapping vertex elements to the corresponding parameters of the vertex shader */
VertexOut VertexShader(Vertex1 vertexIn) 
{
	VertexOut vout;
	//vout.position

	//return 
}