#include "Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer
      Summary:  Constructor
      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer() :
        m_driverType(D3D_DRIVER_TYPE_NULL), m_featureLevel(D3D_FEATURE_LEVEL_10_0),
        m_d3dDevice(nullptr), m_d3dDevice1(nullptr),
        m_immediateContext(nullptr), m_immediateContext1(nullptr),
        m_swapChain(nullptr),m_swapChain1(nullptr),
        m_renderTargetView(nullptr),
        m_vertexShaders(nullptr),
        m_pixelShaders(nullptr)
    {};
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize
      Summary:  Creates Direct3D device and swap chain
      Args:     HWND hWnd
                  Handle to the window
      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                  m_d3dDevice1, m_immediateContext1, m_swapChain1,
                  m_swapChain, m_renderTargetView].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd) {
        HRESULT hr = S_OK;

        D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        };
        UINT createDeviceFlags = 0;

#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        this->m_featureLevel = D3D_FEATURE_LEVEL_11_0;
        this->m_driverType = D3D_DRIVER_TYPE_NULL;
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;
        ComPtr<IDXGISwapChain> swapChain;

        hr = D3D11CreateDevice(
            nullptr,                    // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
            0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            createDeviceFlags,                // Set debug and Direct2D compatibility flags.
            levels,                     // List of feature levels this app can support.
            ARRAYSIZE(levels),          // Size of the list above.
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            &device,                    // Returns the Direct3D device created.
            &m_featureLevel,            // Returns feature level of device created.
            &context                    // Returns the device immediate context.
        );

        if (FAILED(hr)) return (hr);
        device.As(&m_d3dDevice);
        context.As(&m_immediateContext);
        //스왑체인 디스크립션

        DXGI_SWAP_CHAIN_DESC desc = {

        .BufferDesc = {
            .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
        },

        .SampleDesc = {
            .Count = 1,
            .Quality = 0,
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .OutputWindow = hWnd,
        .Windowed = TRUE,


        };
        
        Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
        m_d3dDevice.As(&dxgiDevice);
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        Microsoft::WRL::ComPtr<IDXGIFactory> factory;

        hr = dxgiDevice->GetAdapter(&adapter);

        if (SUCCEEDED(hr))
        {
            adapter->GetParent(IID_PPV_ARGS(&factory));
            hr = factory->CreateSwapChain(
                m_d3dDevice.Get(),
                &desc,
                m_swapChain.GetAddressOf()

            );
        }
        
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            static_cast<UINT> (800),
            static_cast<UINT> (600),
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL
        );

        m_d3dDevice->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            m_depthStencil.GetAddressOf()
        );

        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
        if (&depthStencilViewDesc == NULL) {
            return 0;
        }
        //깊이 스텐실
        hr = m_d3dDevice->CreateDepthStencilView(
            m_depthStencil.Get(),
            &depthStencilViewDesc,
            m_depthStencilView.GetAddressOf()
        );
        
        D3D11_TEXTURE2D_DESC    m_bbDesc;
        D3D11_VIEWPORT          m_viewport = {
        .Width = (float)600,
        .Height = (float)800,
        .MinDepth = 0,
        .MaxDepth = 1,
        };
        ComPtr<ID3D11Texture2D> pBackBuffer;

        hr = m_swapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            (void**)pBackBuffer.GetAddressOf());

        pBackBuffer->GetDesc(&m_bbDesc);

        hr = m_d3dDevice->CreateRenderTargetView(
            pBackBuffer.Get(),
            nullptr,
            m_renderTargetView.GetAddressOf()
        );
        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
        m_immediateContext->RSSetViewports(
            1,
            &m_viewport
        );
        
        //CREATE VIEW AND PROJECTION MATRICS
        
        XMVECTOR eye(0.0, 1.0, -5.0, 0.0);
        XMVECTOR at(0.0, 1.0, 0.0, 0.0);
        XMVECTOR up(0.0, 1.0, 0.0, 0.0);
        XMMATRIX V = XMMatrixLookAtLH(eye, at, up);
        

        
        return hr;

    }
 
    HRESULT Renderer::AddRenderable(_In_ PCWSTR pszRenderableName, _In_ const std::shared_ptr<Renderable>& renderable) {
        //Add some Object(Renderable) into Renderables Unordered map
        if (m_renderables.contains(pszRenderableName)) {
            return E_FAIL;
        }
        else {
            m_renderables.insert(std::make_pair(pszRenderableName, renderable));
            return S_OK;
        }
        
    };
    HRESULT Renderer::AddVertexShader(_In_ PCWSTR pszVertexShaderName, _In_ const std::shared_ptr<VertexShader>& vertexShader) {
        //Add some VertexShader into Renderables Unordered map
        if (m_vertexShaders.contains(pszVertexShaderName)) {
            return E_FAIL;
        }
        else {
            m_vertexShaders.insert(std::make_pair(pszVertexShaderName, vertexShader));
            return S_OK;
        }
    };
    HRESULT Renderer::AddPixelShader(_In_ PCWSTR pszPixelShaderName, _In_ const std::shared_ptr<PixelShader>& pixelShader) {
        //Add some PixelShader into Renderables Unordered map
        if (m_pixelShaders.contains(pszPixelShaderName)) {
            return E_FAIL;
        }
        else {
            m_pixelShaders.insert(std::make_pair(pszPixelShaderName, pixelShader));
            return S_OK;
        }
    };

    void Renderer::Update(_In_ FLOAT deltaTime) {
        std::unordered_map<PCWSTR, std::shared_ptr<Renderable>>::iterator it;
        for (it = m_renderables.begin(); it != m_renderables.end(); it++) 
        {
            it->second->Update(deltaTime);
        }
        
    };
    void Renderer::Render() {
        //set Shaders
        float ClearColor[4] = { 0.0f, 0.0f, 0.6f, 1.0f };
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), ClearColor);
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), NULL, 1.0, D3D10_CLEAR_STENCIL);

        std::unordered_map<PCWSTR, std::shared_ptr<Renderable>>::iterator it;
        for (it = m_renderables.begin(); it != m_renderables.end(); it++)
        {
            //set buffer
            UINT stride = sizeof(SimpleVertex);
            UINT offset = 0;
            m_immediateContext->IASetVertexBuffers(0, 1, it->second->GetVertexBuffer().GetAddressOf(), &stride, &offset);
            m_immediateContext->IASetIndexBuffer(it->second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(it->second->GetVertexLayout().Get());
            //Update Constant buffer
            m_immediateContext->VSSetConstantBuffers(0, 3, it->second->GetConstantBuffer().GetAddressOf());
            //Render The Triangle
            m_immediateContext->Draw(3, 0);
        }
        
        m_swapChain->Present(1, 0);
    };

    HRESULT Renderer::SetVertexShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszVertexShaderName) {
        // 해당 renderable이 있는지?
        if (m_renderables.contains(pszRenderableName)) {
            return E_FAIL;
        }
        // 해당 renderable에 추가하기
        else {
            // 해당 VertexShaderName이 있는지?
            if (m_vertexShaders.contains(pszVertexShaderName)) {
                m_renderables.find(pszRenderableName)->second->SetVertexShader(m_vertexShaders.find(pszVertexShaderName)->second);
                return S_OK;
            }
            // 해당 VertexShader가 없으면 실패
            return E_FAIL;
            
        }
        
    }
    HRESULT Renderer::SetPixelShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszPixelShaderName) {
        // 해당 renderable이 있는지?
        if (m_renderables.contains(pszRenderableName)) {
            return E_FAIL;
        }
        // 해당 renderable에 추가하기
        else {
            // 해당 PixelShaderName이 있는지?
            if (m_vertexShaders.contains(pszPixelShaderName)) {
                m_renderables.find(pszRenderableName)->second->SetPixelShader(m_pixelShaders.find(pszPixelShaderName)->second);
                return S_OK;
            }
            // 해당 PixelShader가 없으면 실패
            return E_FAIL;

        }
    };
    D3D_DRIVER_TYPE Renderer::GetDriverType() const {
        return m_driverType;
    };

}