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
        m_driverType(D3D_DRIVER_TYPE_NULL), m_featureLevel(D3D_FEATURE_LEVEL_11_0),
        m_d3dDevice(nullptr), m_d3dDevice1(nullptr),
        m_immediateContext(nullptr), m_immediateContext1(nullptr),
        m_swapChain(nullptr), m_swapChain1(nullptr),
        m_renderTargetView(nullptr),
        m_camera(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
        m_projection(),
        m_renderables(std::unordered_map<std::wstring, std::shared_ptr<Renderable>>()),
        m_vertexShaders(std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>()),
        m_pixelShaders(std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>()) {};
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

        RECT rc;
        GetClientRect(hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        DWORD createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        // Create Direct3D 11 device and context
        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
    };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr))
            {
                break;
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }


        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory = nullptr;
        {
            Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice = nullptr;
            hr = m_d3dDevice.As(&dxgiDevice);
            if (SUCCEEDED(hr))
            {
                Microsoft::WRL::ComPtr<IDXGIAdapter> adapter = nullptr;
                hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(__uuidof(IDXGIFactory1), &dxgiFactory);
                    adapter.Reset();
                }
                dxgiDevice.Reset();
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        // Create swap chain
        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;
        hr = dxgiFactory.As(&dxgiFactory2);

        if (dxgiFactory2)
        {
            // DirectX 11.1 or later
            hr = m_d3dDevice.As(&m_d3dDevice1);

            if (SUCCEEDED(hr))
            {
                hr = m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd = {
                .Width = width,
                .Height = height,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = {
                    .Count = 1,
                    .Quality = 0
                },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1
            };

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }

            dxgiFactory2.Reset();
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd = {
                .BufferDesc = {
                    .Width = width,
                    .Height = height,
                    .RefreshRate = {
                        .Numerator = 60,
                        .Denominator = 1
                    },
                    .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                },
                .SampleDesc = {
                    .Count = 1,
                    .Quality = 0
                },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1,
                .OutputWindow = hWnd,
                .Windowed = TRUE
            };

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        ///////////////////////////////////////////////
        // Create RenderTargetView
        ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;

        hr = m_swapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            &pBackBuffer);

        if (FAILED(hr)) return (hr);

        hr = m_d3dDevice->CreateRenderTargetView(
            pBackBuffer.Get(),
            nullptr,
            m_renderTargetView.GetAddressOf()
        );
        pBackBuffer.Reset();
        if (FAILED(hr)) return (hr);
        ///////////////////////////////////////////////
        //Create Depth stencil texture
        CD3D11_TEXTURE2D_DESC depthStencilDesc = {};
        
        depthStencilDesc.Width = width;
        depthStencilDesc.Height = height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;


        m_d3dDevice->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            m_depthStencil.GetAddressOf()
        );
        ///////////////////////////////////////////////
        //Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {
        .Format = depthStencilDesc.Format,
        .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
        .Texture2D = {
            .MipSlice = 0,
        }
        };
        
        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, m_depthStencilView.GetAddressOf());
        ///////////////////////////////////////////////
        //OM SET RENDER TARTGET
        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        ///////////////////////////////////////////////
        // Setup the viewport
        D3D11_VIEWPORT vp = {
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = (float)width,
            .Height = (float)height,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };
        m_immediateContext->RSSetViewports(
            1,
            &vp
        );
       
        //CREATE VIEW AND PROJECTION MATRICS(IN RENDERER)
        //Renderer->renderables 
        XMVECTOR eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
        XMVECTOR at = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMMATRIX V = XMMatrixLookAtLH(eye, at, up);
        //m_view = V;
        m_projection = XMMatrixPerspectiveFovLH(
            XM_PIDIV2,
            (FLOAT) width / (FLOAT)height,
            0.01f,
            100.0f
        );
        CBChangeOnResize cb;
        cb.Projection = XMMatrixTranspose(m_projection);

        //
        

        D3D11_BUFFER_DESC bd;
        bd.ByteWidth = sizeof(CBChangeOnResize);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &cb;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;

        hr = m_d3dDevice->CreateBuffer(
            &bd,
            &initData,
            m_cbChangeOnResize.GetAddressOf()
        );
        if (FAILED(hr))
        {
            return hr;
        }

        
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cb, 0, 0);
        //


        std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>::iterator itVertexShader;

        for (itVertexShader = m_vertexShaders.begin(); itVertexShader != m_vertexShaders.end(); itVertexShader++)
        {
            itVertexShader->second->Initialize(m_d3dDevice.Get());
        }


        std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>::iterator itPixelShader;

        for (itPixelShader = m_pixelShaders.begin(); itPixelShader != m_pixelShaders.end(); itPixelShader++)
        {
            itPixelShader->second->Initialize(m_d3dDevice.Get());
        }

        std::unordered_map<std::wstring, std::shared_ptr<Renderable>>::iterator it;

        for (it = m_renderables.begin(); it != m_renderables.end(); it++)
        {
            //Init Randerables...!! 
            //In init, creates Buffers
            //so, renderables finally have shader,buffers,world matrix

            it->second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());

            //SetVertexShaderOfRenderable
        }

        m_camera.Initialize(m_d3dDevice.Get());

        CBLights cl;

        D3D11_BUFFER_DESC bdl;
        bdl.ByteWidth = sizeof(CBLights);
        bdl.Usage = D3D11_USAGE_DEFAULT;
        bdl.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bdl.CPUAccessFlags = 0;
        bdl.MiscFlags = 0;
        bdl.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initDatal;
        initDatal.pSysMem = &cl;
        initDatal.SysMemPitch = 0;
        initDatal.SysMemSlicePitch = 0;

        hr = m_d3dDevice->CreateBuffer(
            &bdl,
            &initDatal,
            m_cbLights.GetAddressOf()
        );
        m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0, nullptr, &cl, 0, 0);
        if (FAILED(hr))
        {
            return hr;
        }

        //init Scene and Their Voxels(Scene have Voxel Initialize)
        for (auto& it : m_scenes) {
            it.second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());

        }

        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        return S_OK;

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
        std::unordered_map<std::wstring, std::shared_ptr<Renderable>>::iterator it;
        for (it = m_renderables.begin(); it != m_renderables.end(); it++) 
        {
            it->second->Update(deltaTime*(FLOAT)0.0001);
        }
        m_camera.Update(deltaTime);
        for (int i = 0; i < NUM_LIGHTS; i++) {
            m_aPointLights[i]->Update(deltaTime * (FLOAT)0.0001);
        };
    };
    void Renderer::Render() {
        //CLEAR RENDER TARGET VIEW AND DEPTHSTENCIL!!
        float ClearColor[4] = { 0.0f, 0.0f, 0.6f, 1.0f };
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), ClearColor);
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH , 1.0f, 0);
        
        //CREATE CAMERA BUFFER//
        CBChangeOnCameraMovement cbc;
        cbc.View = XMMatrixTranspose(m_camera.GetView());
        XMStoreFloat4(&cbc.CameraPosition, m_camera.GetEye());
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cbc, 0, 0);
        
        //UPDATE LIGHT BUFFERS//
                //-------LAB06--------//
        CBLights cbLight;
        for (int i = 0; i < 2; i++) {
            cbLight.LightColors[i] = (m_aPointLights[i]->GetColor());
            cbLight.LightPositions[i] = (m_aPointLights[i]->GetPosition());
        };
        m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0, nullptr, &cbLight, 0, 0);


        std::unordered_map<std::wstring, std::shared_ptr<Renderable>>::iterator it;
        for (it = m_renderables.begin(); it != m_renderables.end(); it++)
        {
            //set bufferN
            UINT stride = sizeof(SimpleVertex);
            UINT offset = 0;

            //Update Constant buffer 
            CBChangesEveryFrame cb;
            cb.World = XMMatrixTranspose(it->second->GetWorldMatrix());
            cb.OutputColor = it->second->GetOutputColor();
            //-------LAB05--------//
            m_immediateContext->UpdateSubresource(it->second->GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);

            //set topology
            
            m_immediateContext->IASetVertexBuffers(0u, 1u, it->second->GetVertexBuffer().GetAddressOf(), &stride, &offset);
            m_immediateContext->IASetIndexBuffer(it->second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(it->second->GetVertexLayout().Get());


            m_immediateContext->VSSetShader(it->second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, it->second->GetConstantBuffer().GetAddressOf());

            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2u, 1u, m_camera.GetConstantBuffer().GetAddressOf());


            m_immediateContext->PSSetShader(it->second->GetPixelShader().Get(), nullptr, 0);
            //-------LAB05-------//
            m_immediateContext->PSSetConstantBuffers(0u, 1u, it->second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, m_camera.GetConstantBuffer().GetAddressOf());

            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

           
            
            if (it->second->HasTexture())
            {
                for (UINT i = 0u; i < it->second->GetNumMeshes(); ++i)
                {
                    const UINT materialIndex = it->second->GetMesh(i).uMaterialIndex;
                    if (it->second->GetMaterial(materialIndex).pDiffuse)
                    {
                        m_immediateContext->PSSetShaderResources(0u, 1u, it->second->GetMaterial(materialIndex).pDiffuse->GetTextureResourceView().GetAddressOf());
                        m_immediateContext->PSSetSamplers(0u, 1u, it->second->GetMaterial(materialIndex).pDiffuse->GetSamplerState().GetAddressOf());
                    }
                    m_immediateContext->DrawIndexed(
                        it->second->GetMesh(i).uNumIndices,
                        it->second->GetMesh(i).uBaseIndex,
                        it->second->GetMesh(i).uBaseVertex);
                }
            }
            else
            {
                m_immediateContext->DrawIndexed(it->second->GetNumIndices(), 0u, 0);
            }
            
            //Assignment02
        for (UINT i = 0u; i < m_scenes.find(m_pszMainSceneName)->second->GetVoxels().size(); i++) {


            //SET INPUT LAYOUT
            UINT stride[2] = { sizeof(SimpleVertex),sizeof(InstanceData) };
            UINT offset[2] = { 0,0 };

            // Update Constant buffer 
            CBChangesEveryFrame cb;
            cb.World = XMMatrixTranspose(m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetWorldMatrix());
            cb.OutputColor = m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetOutputColor();
            m_immediateContext->UpdateSubresource(
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetConstantBuffer().Get(),
                0, nullptr, &cb, 0, 0);


            m_immediateContext->IASetInputLayout(
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetVertexLayout().Get());

            ComPtr<ID3D11Buffer> tmp[2] = 
            { 
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetVertexBuffer(),
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetInstanceBuffer()
            };
            m_immediateContext->IASetVertexBuffers(0, 2,tmp->GetAddressOf(), stride, offset);

            m_immediateContext->IASetIndexBuffer(
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetIndexBuffer().Get(),
                DXGI_FORMAT_R16_UINT, 0);
            

            


            //Set VERTEX SHADER
            m_immediateContext->VSSetShader(
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetVertexShader().Get()
                , nullptr
                , 0);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetConstantBuffer().GetAddressOf());

            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2u, 1u, m_camera.GetConstantBuffer().GetAddressOf());

            //Set PIXEL SHADER
            m_immediateContext->PSSetShader(
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetPixelShader().Get(),
                nullptr, 0);
            m_immediateContext->PSSetConstantBuffers(0u, 1u, m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, m_camera.GetConstantBuffer().GetAddressOf());

            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());
            //DRAW INSTNACE
            m_immediateContext->DrawIndexedInstanced(
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetNumIndices(),
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetNumInstances(),
                0u,
                0,
                0u
            );

        }
        }




        m_swapChain->Present(0, 0);
    };
    HRESULT Renderer::AddScene(_In_ PCWSTR pszSceneName, const std::filesystem::path& sceneFilePath) {
        if (!m_scenes.contains(pszSceneName)) {
            //
            m_scenes.insert(std::make_pair(pszSceneName, std::make_shared<library::Scene>(sceneFilePath)));
        }
        else {
            //already exist
            return E_FAIL;
        }
    }

    //Assignments02
    HRESULT Renderer::SetMainScene(_In_ PCWSTR pszSceneName) {
        m_pszMainSceneName = pszSceneName;
        return S_OK;
    }

    HRESULT Renderer::SetVertexShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszVertexShaderName) {
        if (m_scenes.contains(pszSceneName)) {
            if (m_vertexShaders.contains(pszVertexShaderName)) {
                for (auto& it : m_scenes.find(pszSceneName)->second->GetVoxels()) {
                    it->SetVertexShader(m_vertexShaders.find(pszVertexShaderName)->second);
                }
            }
            return S_OK;
        }
        return E_FAIL;
    }
    HRESULT Renderer::SetPixelShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszPixelShaderName) {
        if (m_scenes.contains(pszSceneName)) {
            if (m_pixelShaders.contains(pszPixelShaderName)) {
                for (auto& it : m_scenes.find(pszSceneName)->second->GetVoxels()) {
                    it->SetPixelShader(m_pixelShaders.find(pszPixelShaderName)->second);
                }
            }
            return S_OK;
        }
        return E_FAIL;
    }
    HRESULT Renderer::SetVertexShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszVertexShaderName) {
        // 해당 renderable이 있는지?
        if (!m_renderables.contains(pszRenderableName)) {
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
        if (!m_renderables.contains(pszRenderableName)) {
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
    //어따쓰는걸까용
    //위에????
    D3D_DRIVER_TYPE Renderer::GetDriverType() const {
        return m_driverType;
    };
    void Renderer::HandleInput(_In_ const DirectionsInput& directions, _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime) {
        
        //그냥 받은거 카메라한테 넘겨줌
        m_camera.HandleInput(
            directions,
            mouseRelativeMovement,
            deltaTime
        );
    };

    HRESULT Renderer::AddPointLight(_In_ size_t index, _In_ const std::shared_ptr<PointLight>& pPointLight) {
        if (index > NUM_LIGHTS) {
            return E_FAIL;
        }
        else {
            m_aPointLights[index] = pPointLight;
            return S_OK;
        }
    };

}