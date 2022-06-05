#include "Renderer/Renderable.h"
#include "Texture/DDSTextureLoader.h"
#include <filesystem>
namespace library
{

    Renderable::Renderable(_In_ const XMFLOAT4& outputColor)
        : m_vertexBuffer(nullptr)
        , m_indexBuffer(nullptr)
        , m_constantBuffer(nullptr)
        , m_normalBuffer(nullptr)
        , m_aMeshes(std::vector<BasicMeshEntry>())
        , m_aMaterials(std::vector<std::shared_ptr<Material>>())
        , m_aNormalData(std::vector<NormalData>())
        , m_vertexShader(nullptr)
        , m_pixelShader(nullptr)
        , m_outputColor(outputColor)
        , m_world(XMMatrixIdentity())
        , m_padding()
        , m_bHasNormalMap(FALSE)
    {};

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

        //Normal Mapping
        if (m_aNormalData.empty()) {
            calculateNormalMapVectors();
        }
        D3D11_BUFFER_DESC bufferDescofNormal = {
        .ByteWidth = (sizeof(NormalData) * (UINT)m_aNormalData.size()),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
        .StructureByteStride = 0
        };

        D3D11_SUBRESOURCE_DATA subDataofNormal = {
        .pSysMem = m_aNormalData.data(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0,
        };

        hr = pDevice->CreateBuffer(
            &bufferDescofNormal, 
            &subDataofNormal,
            m_normalBuffer.GetAddressOf());


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
    const std::shared_ptr<Material>& Renderable::GetMaterial(UINT uIndex) const {
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
        
        m_world *= XMMatrixScaling(scaleX, scaleY, scaleZ);
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
    //LAB09


    ComPtr<ID3D11Buffer>& Renderable::GetNormalBuffer() {
        return m_normalBuffer;
    }
    BOOL Renderable::HasNormalMap() const {
        return m_bHasNormalMap;
    }
    void Renderable::calculateNormalMapVectors() {
        UINT uNumFaces = GetNumIndices() / 3;
        const SimpleVertex* aVertices = getVertices();
        const WORD* aIndices = getIndices();

        m_aNormalData.resize(GetNumVertices(), NormalData());

        XMFLOAT3 tangent, bitangent;

        for (int i = 0; i < uNumFaces; ++i)
        {
            calculateTangentBitangent(aVertices[aIndices[i * 3]], aVertices[aIndices[i * 3 + 1]],
                aVertices[aIndices[i * 3 + 2]], tangent, bitangent);

            m_aNormalData[aIndices[i * 3]].Tangent = tangent;
            m_aNormalData[aIndices[i * 3]].Bitangent = bitangent;

            m_aNormalData[aIndices[i * 3 + 1]].Tangent = tangent;
            m_aNormalData[aIndices[i * 3 + 1]].Bitangent = bitangent;

            m_aNormalData[aIndices[i * 3 + 2]].Tangent = tangent;
            m_aNormalData[aIndices[i * 3 + 2]].Bitangent = bitangent;
        }
    }
    void Renderable::calculateTangentBitangent(_In_ const SimpleVertex& v1, _In_ const SimpleVertex& v2, _In_ const SimpleVertex& v3, _Out_ XMFLOAT3& tangent, _Out_ XMFLOAT3& bitangent) {
        XMFLOAT3 vector1, vector2;
        XMFLOAT2 tuVector, tvVector;

        // Calculate the two vectors for this face.
        vector1.x = v2.Position.x - v1.Position.x;
        vector1.y = v2.Position.y - v1.Position.y;
        vector1.z = v2.Position.z - v1.Position.z;

        vector2.x = v3.Position.x - v1.Position.x;
        vector2.y = v3.Position.y - v1.Position.y;
        vector2.z = v3.Position.z - v1.Position.z;

        // Calculate the tu and tv texture space vectors.
        tuVector.x = v2.TexCoord.x - v1.TexCoord.x;
        tvVector.x = v2.TexCoord.y - v1.TexCoord.y;

        tuVector.y = v3.TexCoord.x - v1.TexCoord.x;
        tvVector.y = v3.TexCoord.y - v1.TexCoord.y;

        // Calculate the denominator of the tangent/binormal equation.
        float den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

        // Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
        tangent.x = (tvVector.y * vector1.x - tvVector.x * vector2.x) * den;
        tangent.y = (tvVector.y * vector1.y - tvVector.x * vector2.y) * den;
        tangent.z = (tvVector.y * vector1.z - tvVector.x * vector2.z) * den;

        bitangent.x = (tuVector.x * vector2.x - tuVector.y * vector1.x) * den;
        bitangent.y = (tuVector.x * vector2.y - tuVector.y * vector1.y) * den;
        bitangent.z = (tuVector.x * vector2.z - tuVector.y * vector1.z) * den;

        // Calculate the length of this normal.
        float length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

        // Normalize the normal and then store it
        tangent.x = tangent.x / length;
        tangent.y = tangent.y / length;
        tangent.z = tangent.z / length;

        // Calculate the length of this normal.
        length = sqrt((bitangent.x * bitangent.x) + (bitangent.y * bitangent.y) + (bitangent.z * bitangent.z));

        // Normalize the normal and then store it
        bitangent.x = bitangent.x / length;
        bitangent.y = bitangent.y / length;
        bitangent.z = bitangent.z / length;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::AddMaterial
      Summary:  Add material to this renderable
      Args:     std::shared_ptr<Material>& material
                  Material to add
      Modifies: [m_aMaterials]
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::AddMaterial(_In_ const std::shared_ptr<Material>& material)
    {
        m_aMaterials.push_back(material);
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetMaterialOfMesh
      Summary:  Set the material of the mesh
      Args:     const UINT uMeshIndex
                  Index of the mesh
                 const UINT uMaterialIndex
                  Index of the material
      Modifies: [m_aMeshes, m_bHasNormalMap]
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderable::SetMaterialOfMesh(_In_ const UINT uMeshIndex, _In_ const UINT uMaterialIndex)
    {
        if (uMeshIndex >= m_aMeshes.size() || uMaterialIndex >= m_aMaterials.size())
        {
            return E_FAIL;
        }

        m_aMeshes[uMeshIndex].uMaterialIndex = uMaterialIndex;

        if (m_aMaterials[uMeshIndex]->pNormal)
        {
            m_bHasNormalMap = true;
        }

        return S_OK;
    }

}
