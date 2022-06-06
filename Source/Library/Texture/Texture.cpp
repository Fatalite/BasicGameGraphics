#include "Texture.h"

#include "Texture/DDSTextureLoader.h"
#include "Texture/WICTextureLoader.h"

namespace library
{
    ComPtr<ID3D11SamplerState> Texture::s_samplers[static_cast<size_t>(eTextureSamplerType::COUNT)];


    Texture::Texture(_In_ const std::filesystem::path& filePath)
        : m_filePath(filePath)
        , m_textureRV(nullptr)
        , m_samplerLinear(nullptr)
        , m_textureSamplerType(){};

    // Should be called once to load the texture
    HRESULT Texture::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) {
        //텍스처 파라미터 널포인터로? 왜?
        HRESULT hr = CreateWICTextureFromFile(
            pDevice,
            pImmediateContext,
            m_filePath.c_str(),
            nullptr,
            m_textureRV.GetAddressOf()
        );
        if (FAILED(hr))
        {
            hr = CreateDDSTextureFromFile(pDevice, m_filePath.c_str(), nullptr, m_textureRV.GetAddressOf());
            if (FAILED(hr))
            {
                OutputDebugString(L"Can't load texture from \"");
                OutputDebugString(m_filePath.c_str());
                OutputDebugString(L"\n");
                return hr;
            }
        }


        //ASSIGNMENT 3
                // Create the sample state
        if (!s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_WRAP)].Get())
        {
            D3D11_SAMPLER_DESC sampDesc =
            {
                sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
                sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
                sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
                sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER,
                sampDesc.MinLOD = 0,
                
                //MaxLOD should required?
            };
            sampDesc.MaxLOD = 1;
            hr = pDevice->CreateSamplerState(&sampDesc, s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_WRAP)].GetAddressOf());
            if (FAILED(hr))
            {
                return hr;
            }
        }

        if (!s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_CLAMP)].Get())
        {
            D3D11_SAMPLER_DESC sampDesc =
            {
                sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
                sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
                sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
                sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER,
                sampDesc.MinLOD = 0,
            };
            hr = pDevice->CreateSamplerState(&sampDesc, s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_CLAMP)].GetAddressOf());
            if (FAILED(hr))
            {
                return hr;
            }
        }

    };
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::GetTextureResourceView
      Summary:  Constructor
      Returns:  ComPtr<ID3D11ShaderResourceView>&
                  Shader resource view
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11ShaderResourceView>& Texture::GetTextureResourceView()
    {
        return m_textureRV;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::GetSamplerState
      Summary:  Constructor
      Returns:  ComPtr<ID3D11SamplerState>&
                  Sampler state
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11SamplerState>& Texture::GetSamplerState()
    {
        return m_samplerLinear;
    }

    Texture::eTextureSamplerType Texture::GetSamplerType() const {
        return m_textureSamplerType;
    }

}