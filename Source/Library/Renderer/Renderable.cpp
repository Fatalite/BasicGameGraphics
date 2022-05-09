#include "Renderer/Renderable.h"
#include "Texture/DDSTextureLoader.h"
#include <filesystem>
namespace library
{

    Renderable::Renderable(_In_ const XMFLOAT4& outputColor)
        : m_vertexBuffer(nullptr)
        , m_indexBuffer(nullptr)
        , m_constantBuffer(nullptr)
        , m_aMeshes(std::vector<BasicMeshEntry>())
        , m_aMaterials(std::vector<Material>())
        , m_vertexShader(nullptr)
        , m_pixelShader(nullptr)
        , m_outputColor(outputColor)
        , m_padding()
        , m_world(XMMatrixIdentity()) {};

    HRESULT Renderable::initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) {
        //Create the vertex buffer
        D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = (sizeof(SimpleVertex) * GetNumVertices()),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
        .StructureByteStride = 0
        };

        D3D11_SUBRESOURCE_DATA subData = {
        .pSysMem = getVertices(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0,
        };

        HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &subData, m_vertexBuffer.GetAddressOf());
        if (FAILED(hr)) return (hr);
        //Create the index buffer

        //!!!!!!!!!WARNING WORD IS DATATYPE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //³¢¤Á°¼°¼¤Á¤Á°¼¤Á¤·
        D3D11_BUFFER_DESC indexDesc;
        indexDesc.Usage = D3D11_USAGE_DEFAULT;
        indexDesc.ByteWidth = sizeof(WORD) * GetNumIndices();
        indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexDesc.CPUAccessFlags = 0;
        indexDesc.MiscFlags = 0;
        indexDesc.StructureByteStride = 0;

        //define idxbuffer subresource
        D3D11_SUBRESOURCE_DATA idxsubData;
        //idxsubData.pSysMem = idxVertexID;
        idxsubData.pSysMem = getIndices();
        idxsubData.SysMemPitch = 0;
        idxsubData.SysMemSlicePitch = 0;


        //make indexbuffer 
        hr = pDevice->CreateBuffer(&indexDesc, &idxsubData, m_indexBuffer.GetAddressOf());
        if (FAILED(hr)) return (hr);


        //Create the constant buffer and init WorldMatrix
        CBChangesEveryFrame cb;

        D3D11_BUFFER_DESC bd;
        bd.ByteWidth = sizeof(CBChangesEveryFrame);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &cb;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;

        hr = pDevice->CreateBuffer(
            &bd,
            &initData,
            m_constantBuffer.GetAddressOf()
        );
        if (FAILED(hr))
        {
            return hr;
        }
         cb.World = XMMatrixTranspose(XMMatrixIdentity());
        //cb.OutputColor = m_outputColor;
        //pImmediateContext->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);


        return S_OK;
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
    //-------LAB07--------//
    const Material& Renderable::GetMaterial(UINT uIndex) const {
        //¹üÀ§¿¡ µû¶ó¼­ Áà¾ßÇÏ´ÂÁö?
        if (uIndex >= m_aMaterials.size()) {
            assert("NONONONO!!");
        }
        return m_aMaterials[uIndex];
    };
    const Renderable::BasicMeshEntry& Renderable::GetMesh(UINT uIndex) const {
        if (uIndex >= m_aMeshes.size()) {
            assert("NONONONO!!");
        }
        return m_aMeshes[uIndex];
    };
    UINT Renderable::GetNumMeshes() const {
        return static_cast<UINT>(m_aMeshes.size());
    }
    UINT Renderable::GetNumMaterials() const {
        return static_cast<UINT>(m_aMaterials.size());
    }
    /*--------------------------------------------------------------------
  TODO: Renderable::GetWorldMatrix definition (remove the comment)
--------------------------------------------------------------------*/

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Renderable::RotateX
  Summary:  Rotates around the x-axis
  Args:     FLOAT angle
              Angle of rotation around the x-axis, in radians
  Modifies: [m_world].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::RotateX(_In_ FLOAT angle)
    {
        m_world *= XMMatrixRotationX(angle);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::RotateY
      Summary:  Rotates around the y-axis
      Args:     FLOAT angle
                  Angle of rotation around the y-axis, in radians
      Modifies: [m_world].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::RotateY(_In_ FLOAT angle)
    {
        m_world *= XMMatrixRotationY(angle); 
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::RotateZ
      Summary:  Rotates around the z-axis
      Args:     FLOAT angle
                  Angle of rotation around the z-axis, in radians
      Modifies: [m_world].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::RotateZ(_In_ FLOAT angle)
    {
         m_world *= XMMatrixRotationZ(angle);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::RotateRollPitchYaw
      Summary:  Rotates based on a given pitch, yaw, and roll (Euler angles)
      Args:     FLOAT pitch
                  Angle of rotation around the x-axis, in radians
                FLOAT yaw
                  Angle of rotation around the y-axis, in radians
                FLOAT roll
                  Angle of rotation around the z-axis, in radians
      Modifies: [m_world].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::RotateRollPitchYaw(_In_ FLOAT pitch, _In_ FLOAT yaw, _In_ FLOAT roll)
    {
        m_world = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::Scale
      Summary:  Scales along the x-axis, y-axis, and z-axis
      Args:     FLOAT scaleX
                  Scaling factor along the x-axis.
                FLOAT scaleY
                  Scaling factor along the y-axis.
                FLOAT scaleZ
                  Scaling factor along the z-axis.
      Modifies: [m_world].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::Scale(_In_ FLOAT scaleX, _In_ FLOAT scaleY, _In_ FLOAT scaleZ)
    {
        m_world =  XMMatrixScaling(scaleX, scaleY, scaleZ);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::Translate
      Summary:  Translates matrix from a vector
      Args:     const XMVECTOR& offset
                  3D vector describing the translations along the x-axis, y-axis, and z-axis
      Modifies: [m_world].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::Translate(_In_ const XMVECTOR& offset)
    {
        m_world *= XMMatrixTranslationFromVector(offset);
    }
        /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetTextureResourceView
      Summary:  Returns the texture resource view
      Returns:  ComPtr<ID3D11ShaderResourceView>&
                  The texture resource view
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetTextureResourceView definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetSamplerState
      Summary:  Returns the sampler state
      Returns:  ComPtr<ID3D11SamplerState>&
                  The sampler state
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderable::GetSamplerState definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMFLOAT4& Renderable::GetOutputColor() const {
        return m_outputColor;
    };
    BOOL Renderable::HasTexture() const
    {
        if (m_aMaterials.size() > 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}
