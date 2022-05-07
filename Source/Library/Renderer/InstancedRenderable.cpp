#include "Renderer/InstancedRenderable.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::InstancedRenderable
      Summary:  Constructor
      Args:     const XMFLOAT4& outputColor
                  Default color of the renderable
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    InstancedRenderable::InstancedRenderable(_In_ const XMFLOAT4& outputColor)
        :Renderable(outputColor), m_padding()
    {}
    InstancedRenderable::InstancedRenderable
    (_In_ std::vector<InstanceData>&& aInstanceData, _In_ const XMFLOAT4& outputColor) 
        :Renderable(outputColor), 
        m_padding(), m_instanceBuffer(nullptr)
       

    {
       
    }
    void InstancedRenderable::SetInstanceData(_In_ std::vector<InstanceData>&& aInstanceData) {
        m_aInstanceData = std::move(aInstanceData);
    }

    ComPtr<ID3D11Buffer>& InstancedRenderable::GetInstanceBuffer() {
        return m_instanceBuffer;
    }
    UINT InstancedRenderable::GetNumInstances() const {
        return m_aInstanceData.size() ;
    }

    HRESULT InstancedRenderable::initializeInstance(_In_ ID3D11Device* pDevice) {

        
        D3D11_BUFFER_DESC bd;
        bd.ByteWidth = m_aInstanceData.size() * sizeof(InstanceData);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
        
        D3D11_SUBRESOURCE_DATA initData = {
            .pSysMem = &m_aInstanceData[0],
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0
        };

        HRESULT hr = pDevice->CreateBuffer(
            &bd,
            &initData,
            m_instanceBuffer.GetAddressOf()
        );

        if (FAILED(hr))
        {
            return hr;
        }

    }

}