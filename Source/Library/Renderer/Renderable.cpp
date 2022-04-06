#include "Renderer/Renderable.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::initialize
      Summary:  Initializes the buffers and the world matrix
      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers
      Modifies: [m_vertexBuffer, m_indexBuffer, m_constantBuffer,
                  m_world].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::initialize definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetVertexShader
      Summary:  Sets the vertex shader to be used for this renderable
                object
      Args:     const std::shared_ptr<VertexShader>& vertexShader
                  Vertex shader to set to
      Modifies: [m_vertexShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::SetVertexShader definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetPixelShader
      Summary:  Sets the pixel shader to be used for this renderable
                object
      Args:     const std::shared_ptr<PixelShader>& pixelShader
                  Pixel shader to set to
      Modifies: [m_pixelShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::SetPixelShader definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexShader
      Summary:  Returns the vertex shader
      Returns:  ComPtr<ID3D11VertexShader>&
                  Vertex shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetVertexShader definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetPixelShader
      Summary:  Returns the vertex shader
      Returns:  ComPtr<ID3D11PixelShader>&
                  Pixel shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetPixelShader definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexLayout
      Summary:  Returns the vertex input layout
      Returns:  ComPtr<ID3D11InputLayout>&
                  Vertex input layout
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetVertexLayout definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexBuffer
      Summary:  Returns the vertex buffer
      Returns:  ComPtr<ID3D11Buffer>&
                  Vertex buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetVertexBuffer definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetIndexBuffer
      Summary:  Returns the index buffer
      Returns:  ComPtr<ID3D11Buffer>&
                  Index buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetIndexBuffer definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetConstantBuffer
      Summary:  Returns the constant buffer
      Returns:  ComPtr<ID3D11Buffer>&
                  Constant buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetConstantBuffer definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetWorldMatrix
      Summary:  Returns the world matrix
      Returns:  const XMMATRIX&
                  World matrix
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetWorldMatrix definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderable::initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) {
        //Create the vertex buffer
        D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = (sizeof(SimpleVertex) * 3),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        .MiscFlags = 0,
        .StructureByteStride = 0
        };

        SimpleVertex vertices[] = {
            {XMFLOAT3(0.0f, 0.0f, 0.5f)},
            {XMFLOAT3(0.5f,-0.5f, 0.5f)},
            {XMFLOAT3(-0.5f,-0.5f, 0.5f) },

        };

        D3D11_SUBRESOURCE_DATA subData = {
        .pSysMem = vertices,
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0,
        };

        HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &subData, m_vertexBuffer.GetAddressOf());
        if (FAILED(hr)) return (hr);
        //Create the index buffer
        D3D11_BUFFER_DESC indexDesc;
        indexDesc.Usage = D3D11_USAGE_DEFAULT;
        indexDesc.ByteWidth = sizeof(SimpleVertex) * 36;
        indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexDesc.CPUAccessFlags = 0;
        indexDesc.MiscFlags = 0;
        indexDesc.StructureByteStride = 0;

        //indexbuffer subresource
        UINT idxVertexID[] = {
            0,1,3, 1,2,3, 1,5,2, 5,6,2, 5,4,6, 4,7,6,
            4,5,0, 5,1,0, 4,0,7, 0,3,7, 3,2,7, 2,6,7
        };
        //define idxbuffer subresource
        D3D11_SUBRESOURCE_DATA idxsubData;
        idxsubData.pSysMem = idxVertexID;
        idxsubData.SysMemPitch = 0;
        idxsubData.SysMemSlicePitch = 0;


        //make indexbuffer 
        hr = pDevice->CreateBuffer(&indexDesc, &idxsubData, m_indexBuffer.GetAddressOf());
        if (FAILED(hr)) return (hr);
        //Create the constant buffer
        //Init the world matrix
        m_world;




        //set topology
        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    };
    void Renderable::SetVertexShader(_In_ const std::shared_ptr<VertexShader>& vertexShader) {
        m_vertexShader = vertexShader;
    }
    void Renderable::SetPixelShader(_In_ const std::shared_ptr<PixelShader>& pixelShader) {
        m_pixelShader = pixelShader;
    }

    ComPtr<ID3D11VertexShader>& Renderable::GetVertexShader() {
        return m_vertexShader->GetVertexShader();
    }
    ComPtr<ID3D11PixelShader>& Renderable::GetPixelShader() {
        return m_pixelShader->GetPixelShader();
    }
    ComPtr<ID3D11InputLayout>& Renderable::GetVertexLayout() {
        return m_vertexShader->GetVertexLayout();
    }
    ComPtr<ID3D11Buffer>& Renderable::GetVertexBuffer() {
        return m_vertexBuffer;
    }
    ComPtr<ID3D11Buffer>& Renderable::GetIndexBuffer() {
        return m_indexBuffer;
    }
    ComPtr<ID3D11Buffer>& Renderable::GetConstantBuffer() {
        return m_constantBuffer;
    }
    const XMMATRIX& Renderable::GetWorldMatrix() const{
        return m_world;
    };
}