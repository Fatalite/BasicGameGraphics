#include "Renderer/Renderer.h"

namespace library
{

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer
      Summary:  Constructor
      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView, m_vertexShader,
                  m_pixelShader, m_vertexLayout, m_vertexBuffer].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Renderer::Renderer()
        : m_driverType(D3D_DRIVER_TYPE_NULL)
        , m_featureLevel(D3D_FEATURE_LEVEL_11_0)
        , m_d3dDevice()
        , m_d3dDevice1()
        , m_immediateContext()
        , m_immediateContext1()
        , m_swapChain()
        , m_swapChain1()
        , m_renderTargetView()
        , m_depthStencil()
        , m_depthStencilView()
        , m_cbChangeOnResize()
        , m_pszMainSceneName(nullptr)
        , m_padding{ '\0' }
        , m_camera(XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f))
        , m_projection()
        , m_scenes()
        , m_invalidTexture(std::make_shared<Texture>(L"Content/Common/InvalidTexture.png"))
        , m_cbShadowMatrix()
        , m_shadowMapTexture()
        , m_shadowVertexShader(nullptr)
        , m_shadowPixelShader(nullptr)
    {
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize
      Summary:  Creates Direct3D device and swap chain
      Args:     HWND hWnd
                  Handle to the window
      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                  m_d3dDevice1, m_immediateContext1, m_swapChain1,
                  m_swapChain, m_renderTargetView, m_vertexShader,
                  m_vertexLayout, m_pixelShader, m_vertexBuffer].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd)
    {
        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(hWnd, &rc);
        UINT uWidth = static_cast<UINT>(rc.right - rc.left);
        UINT uHeight = static_cast<UINT>(rc.bottom - rc.top);

        UINT uCreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
        uCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, uCreateDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, uCreateDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
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
        ComPtr<IDXGIFactory1> dxgiFactory;
        {
            ComPtr<IDXGIDevice> dxgiDevice;
            hr = m_d3dDevice.As(&dxgiDevice);
            if (SUCCEEDED(hr))
            {
                ComPtr<IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(&adapter);
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
                }
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        // Create swap chain
        ComPtr<IDXGIFactory2> dxgiFactory2;
        hr = dxgiFactory.As(&dxgiFactory2);
        if (SUCCEEDED(hr))
        {
            // DirectX 11.1 or later
            hr = m_d3dDevice.As(&m_d3dDevice1);
            if (SUCCEEDED(hr))
            {
                m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd =
            {
                .Width = uWidth,
                .Height = uHeight,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = {.Count = 1u, .Quality = 0u },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1u
            };

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd =
            {
                .BufferDesc = {.Width = uWidth, .Height = uHeight, .RefreshRate = {.Numerator = 60, .Denominator = 1 }, .Format = DXGI_FORMAT_R8G8B8A8_UNORM },
                .SampleDesc = {.Count = 1, .Quality = 0 },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1u,
                .OutputWindow = hWnd,
                .Windowed = TRUE
            };

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

        if (FAILED(hr))
        {
            return hr;
        }

        // Create a render target view
        ComPtr<ID3D11Texture2D> pBackBuffer;
        hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Create depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth =
        {
            .Width = uWidth,
            .Height = uHeight,
            .MipLevels = 1u,
            .ArraySize = 1u,
            .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
            .SampleDesc = {.Count = 1u, .Quality = 0u },
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_DEPTH_STENCIL,
            .CPUAccessFlags = 0u,
            .MiscFlags = 0u
        };
        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, m_depthStencil.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV =
        {
            .Format = descDepth.Format,
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
            .Texture2D = {.MipSlice = 0 }
        };
        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, m_depthStencilView.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        // Setup the viewport
        D3D11_VIEWPORT vp =
        {
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = static_cast<FLOAT>(uWidth),
            .Height = static_cast<FLOAT>(uHeight),
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };
        m_immediateContext->RSSetViewports(1, &vp);

        // Set primitive topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Create the constant buffers
        D3D11_BUFFER_DESC bd =
        {
            .ByteWidth = sizeof(CBChangeOnResize),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0
        };
        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbChangeOnResize.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        //Create Matrix Buffer
        D3D11_BUFFER_DESC bm =
        {
            .ByteWidth = sizeof(CBShadowMatrix),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0
        };

        hr = m_d3dDevice->CreateBuffer(&bm, nullptr, m_cbShadowMatrix.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }



        ///////////// LAB10 Texture Logic ///////////////////////
        //Make shared by my width and uheight
        m_shadowMapTexture = std::make_shared<RenderTexture>(uWidth,uHeight);

        //Initializing Texture
        m_shadowMapTexture->Initialize(m_d3dDevice.Get() , m_immediateContext.Get());

        // Initialize the projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, static_cast<FLOAT>(uWidth) / static_cast<FLOAT>(uHeight), 0.01f, 1000.0f);

        CBChangeOnResize cbChangesOnResize =
        {
            .Projection = XMMatrixTranspose(m_projection)
        };
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cbChangesOnResize, 0, 0);
        ///////////// LAB10 Logic ///////////////////////

        ///////////// LAB10 Init all lights Logic ///////////////////////
        for (UINT iLight = 0u; iLight < NUM_LIGHTS;  iLight++) {
            m_scenes[m_pszMainSceneName]->GetPointLight(iLight)->Initialize(uWidth, uHeight);
        }

        ///////////// LAB10 Init all lights Logic ///////////////////////
        bd.ByteWidth = sizeof(CBPointLight);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0u;

        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbLights.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        m_camera.Initialize(m_d3dDevice.Get());

        if (!m_scenes.contains(m_pszMainSceneName))
        {
            return E_FAIL;
        }

        hr = m_scenes[m_pszMainSceneName]->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_invalidTexture->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
        if (FAILED(hr))
        {
            return hr;
        }

        return S_OK;
    }
    void Renderer::SetShadowMapShaders(_In_ std::shared_ptr<ShadowVertexShader> vertexShader, _In_ std::shared_ptr<PixelShader> pixelShader) {
        
            m_shadowVertexShader = move(vertexShader);
            m_shadowPixelShader = move(pixelShader);
        
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddScene
      Summary:  Add scene to renderer
      Args:     PCWSTR pszSceneName
                  The name of the scene
                const std::shared_ptr<Scene>&
                  The shared pointer to Scene
      Modifies: [m_scenes].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddScene(_In_ PCWSTR pszSceneName, _In_ const std::shared_ptr<Scene>& scene)
    {
        if (m_scenes.contains(pszSceneName))
        {
            return E_FAIL;
        }

        m_scenes[pszSceneName] = scene;

        return S_OK;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetSceneOrNull
      Summary:  Return scene with the given name or null
      Args:     PCWSTR pszSceneName
                  The name of the scene
      Returns:  std::shared_ptr<Scene>
                  The shared pointer to Scene
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    std::shared_ptr<Scene> Renderer::GetSceneOrNull(_In_ PCWSTR pszSceneName)
    {
        if (m_scenes.contains(pszSceneName))
        {
            return m_scenes[pszSceneName];
        }

        return nullptr;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetMainScene
      Summary:  Set the main scene
      Args:     PCWSTR pszSceneName
                  The name of the scene
      Modifies: [m_pszMainSceneName].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetMainScene(_In_ PCWSTR pszSceneName)
    {
        if (!m_scenes.contains(pszSceneName))
        {
            return E_FAIL;
        }

        m_pszMainSceneName = pszSceneName;

        return S_OK;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::HandleInput
      Summary:  Handle user mouse input
      Args:     DirectionsInput& directions
                MouseRelativeMovement& mouseRelativeMovement
                FLOAT deltaTime
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::HandleInput(_In_ const DirectionsInput& directions, _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime)
    {
        m_camera.HandleInput(directions, mouseRelativeMovement, deltaTime);
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Update
      Summary:  Update the renderables each frame
      Args:     FLOAT deltaTime
                  Time difference of a frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Update(_In_ FLOAT deltaTime)
    {
        m_scenes[m_pszMainSceneName]->Update(deltaTime);

        m_camera.Update(deltaTime);
    }
    /*

    */
    void Renderer::RenderSceneToTexture() {


        //Clear 0 and 2 Cbs
        ComPtr<ID3D11ShaderResourceView> const pSRV[2] = { NULL, NULL };
        m_immediateContext->PSSetShaderResources(0, 2, pSRV->GetAddressOf());
        m_immediateContext->PSSetShaderResources(2, 1, pSRV->GetAddressOf());
        //Change render target to shadow map texture
        m_immediateContext->OMSetRenderTargets(
            1, 
            m_shadowMapTexture->GetRenderTargetView().GetAddressOf(), 
            m_depthStencilView.Get());

        //Just Clear Render target and Stencil View by White Color 
        float whiteColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_immediateContext->ClearRenderTargetView(m_shadowMapTexture->GetRenderTargetView().Get(), whiteColor);
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH , 1.0f, 0);

        //Render on Shadow RenderTargetView...
        for (auto& it : m_scenes[m_pszMainSceneName]->GetRenderables()) {

            
            UINT uStride = static_cast<UINT>(sizeof(SimpleVertex));
            UINT uOffset = 0;
            m_immediateContext->IASetVertexBuffers(0u, 1u,
                it.second->GetVertexBuffer().GetAddressOf(), &uStride, &uOffset);
            m_immediateContext->IASetIndexBuffer(it.second->GetIndexBuffer().Get(),
                DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(m_shadowVertexShader->GetVertexLayout().Get());

            CBShadowMatrix cbShadow = {
                .World = XMMatrixTranspose(it.second->GetWorldMatrix()),
                .View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                .Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
                .IsVoxel = false,
            };
            m_immediateContext->UpdateSubresource(m_cbShadowMatrix.Get(), 0, nullptr, &cbShadow, 0, 0);
            m_immediateContext->VSSetConstantBuffers(0, 1, m_cbShadowMatrix.GetAddressOf());
            m_immediateContext->VSSetShader(m_shadowVertexShader->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->PSSetShader(m_shadowPixelShader->GetPixelShader().Get(), nullptr, 0);
           
            for (UINT i = 0; i < it.second->GetNumMeshes(); i++)
            {
                m_immediateContext->DrawIndexed(
                    it.second->GetMesh(i).uNumIndices,
                    it.second->GetMesh(i).uBaseIndex,
                    it.second->GetMesh(i).uBaseVertex
                );
            }


        }
        //VOXEL RENDERING, INSTANCED
        for (auto& it : m_scenes[m_pszMainSceneName]->GetVoxels()) {

            UINT uUStride[2] = { static_cast<UINT>(sizeof(SimpleVertex)),  static_cast<UINT>(sizeof(InstanceData)) };
            UINT uUOffset[2] = { 0,0 };
            ComPtr<ID3D11Buffer> vertexInstanceBuffers[2] = {it.get()->GetVertexBuffer() ,it.get()->GetInstanceBuffer() };
            m_immediateContext->IASetVertexBuffers(0, 1, vertexInstanceBuffers[0].GetAddressOf(), &uUStride[0], &uUOffset[0]);
            m_immediateContext->IASetVertexBuffers(2, 1, vertexInstanceBuffers[1].GetAddressOf(), &uUStride[1], &uUOffset[1]);
            m_immediateContext->IASetIndexBuffer(it->GetIndexBuffer().Get(),
                DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(m_shadowVertexShader->GetVertexLayout().Get());

            m_immediateContext->VSSetShader(m_shadowVertexShader->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(0, 1, m_cbShadowMatrix.GetAddressOf());
            m_immediateContext->PSSetShader(m_shadowPixelShader->GetPixelShader().Get(), nullptr, 0);

            CBShadowMatrix cbShadow = {
                .World = XMMatrixTranspose(it->GetWorldMatrix()),
                    .View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                .Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
                .IsVoxel = true,
            };
            m_immediateContext->UpdateSubresource(m_cbShadowMatrix.Get(), 0, nullptr, &cbShadow, 0, 0);

            for (UINT i = 0u; i <it.get()->GetNumMeshes(); ++i)
            {
                // Render the triangles
                m_immediateContext->DrawIndexedInstanced(
                    it.get()->GetMesh(i).uNumIndices,
                    it.get()->GetNumInstances(),
                    it.get()->GetMesh(i).uBaseIndex,
                    it.get()->GetMesh(i).uBaseVertex,
                    0u
                );
            }



        }
        //MODEL RENDERING
        for (auto& it : m_scenes[m_pszMainSceneName]->GetModels()) {

            UINT uuuStride = sizeof(SimpleVertex);
            UINT uuuOffset = 0;
            m_immediateContext->IASetVertexBuffers(0u, 1u,
                it.second->GetVertexBuffer().GetAddressOf(), &uuuStride, &uuuOffset);
            m_immediateContext->IASetIndexBuffer(it.second->GetIndexBuffer().Get(),
                DXGI_FORMAT_R16_UINT, 0);

            m_immediateContext->IASetInputLayout(m_shadowVertexShader->GetVertexLayout().Get());
            m_immediateContext->VSSetShader(m_shadowVertexShader->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(0, 1, m_cbShadowMatrix.GetAddressOf());
            //m_immediateContext->PSSetConstantBuffers(0, 1, m_cbShadowMatrix.GetAddressOf());
            m_immediateContext->PSSetShader(m_shadowPixelShader->GetPixelShader().Get(), nullptr, 0);
            
            CBShadowMatrix cbShadow = {
                .World = XMMatrixTranspose(it.second->GetWorldMatrix()),
                .View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                .Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
    .           IsVoxel = false,
            };
            m_immediateContext->UpdateSubresource(m_cbShadowMatrix.Get(), 0, nullptr, &cbShadow, 0, 0);

            for (UINT i = 0; i < it.second->GetNumMeshes(); i++)
            {
                m_immediateContext->DrawIndexed(
                    it.second->GetMesh(i).uNumIndices, 
                    it.second->GetMesh(i).uBaseIndex, 
                    it.second->GetMesh(i).uBaseVertex);
            }

        }
        //m_swapChain->Present(0, 0);
        //Back to Origin Render target
        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
        
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render
      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Render() {





        //CLEAR RENDER TARGET VIEW AND DEPTHSTENCIL
        float ClearColor[4] = { 0.0f, 0.0f, 0.6f, 1.0f };
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), ClearColor);
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
//1-PASS!!!!!!!!!!!!!!!!
        RenderSceneToTexture();
        //2-PASS!!!!!!!!!!!!!!!!
                //CREATE CAMERA BUFFER//
        CBChangeOnCameraMovement cbc;
        cbc.View = XMMatrixTranspose(m_camera.GetView());
        XMStoreFloat4(&cbc.CameraPosition, m_camera.GetEye());
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cbc, 0, 0);


        //CREATE LIGHT BUFFER//
        CBPointLight cbPointLight[2];
        for (int i = 0; i < NUM_LIGHTS; i++)
        {
            cbPointLight[i].Position = m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetPosition();
            cbPointLight[i].Color = m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetColor();
            cbPointLight[i].LightProjections = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetProjectionMatrix());
            cbPointLight[i].LightViews = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetViewMatrix());
            
            FLOAT attenuationDistance = m_scenes[m_pszMainSceneName]->GetPointLight(i)->GetAttenuationDistance();
            FLOAT attenuationDistanceSquared = attenuationDistance * attenuationDistance;
     
            cbPointLight[i].AttenuationDistance = XMFLOAT4(
                attenuationDistance,
                attenuationDistance,
                attenuationDistanceSquared,
                attenuationDistanceSquared
            );
        }
        m_immediateContext->UpdateSubresource(
            m_cbLights.Get(),
            0,
            nullptr,
            &cbPointLight,
            0,
            0
        );

        //Render Skybox! if SkyBox is not nullptr
        if (m_scenes[m_pszMainSceneName]->GetSkyBox() != nullptr) {

            UINT uuuStride = sizeof(SimpleVertex);
            UINT uuuOffset = 0;
            m_immediateContext->IASetVertexBuffers(0u, 1u,
                m_scenes[m_pszMainSceneName]->GetSkyBox()->GetVertexBuffer().GetAddressOf(), &uuuStride, &uuuOffset);
            m_immediateContext->IASetIndexBuffer(m_scenes[m_pszMainSceneName]->GetSkyBox()->GetIndexBuffer().Get(),
                DXGI_FORMAT_R16_UINT, 0);

            CBChangesEveryFrame cb = {
                .World = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetSkyBox()->GetWorldMatrix()),
                .OutputColor = m_scenes[m_pszMainSceneName]->GetSkyBox()->GetOutputColor(),
                .HasNormalMap = m_scenes[m_pszMainSceneName]->GetSkyBox()->HasNormalMap(),
            };
            m_immediateContext->UpdateSubresource(m_scenes[m_pszMainSceneName]->GetSkyBox()->GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);

            m_immediateContext->IASetInputLayout(m_scenes[m_pszMainSceneName]->GetSkyBox()->GetVertexLayout().Get());

            m_immediateContext->VSSetShader(m_scenes[m_pszMainSceneName]->GetSkyBox()->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->PSSetShader(m_scenes[m_pszMainSceneName]->GetSkyBox()->GetPixelShader().Get(), nullptr, 0);


            m_immediateContext->PSSetConstantBuffers(0u, 1u, m_scenes[m_pszMainSceneName]->GetSkyBox()->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());


            m_immediateContext->VSSetConstantBuffers(0u, 1u, m_scenes[m_pszMainSceneName]->GetSkyBox()->GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            //Now, VertexShader needs Lights constant
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

            //Texture!
            if (m_scenes[m_pszMainSceneName]->GetSkyBox()->HasTexture())
            {
                for (UINT i = 0u; i < m_scenes[m_pszMainSceneName]->GetSkyBox()->GetNumMeshes(); ++i)
                {
                    const UINT materialIndex = m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMesh(i).uMaterialIndex;
                    if (m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse)
                    {
                        ComPtr<ID3D11ShaderResourceView> shaderResources =
                            m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse->GetTextureResourceView();
                        ComPtr<ID3D11SamplerState> samplerStates =
                            m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse->GetSamplerState();
                        m_immediateContext->PSSetShaderResources(0, 1, shaderResources.GetAddressOf());
                        ////////////////////////////////////////
                        Texture::eTextureSamplerType textureSamplerType =
                            m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse->GetSamplerType();
                        m_immediateContext->PSSetSamplers(
                            0u,
                            1u,
                            Texture::s_samplers[static_cast <size_t>(textureSamplerType)].GetAddressOf());
                        ////////////////////////////////////////


                        m_immediateContext->DrawIndexed(
                            m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMesh(i).uNumIndices,
                            m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMesh(i).uBaseIndex,
                            m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMesh(i).uBaseVertex);
                    }
                }
            }
            else
            {
                m_immediateContext->DrawIndexed(m_scenes[m_pszMainSceneName]->GetSkyBox()->GetNumIndices(), 0u, 0);
            }
        }



        //////////////////////////////////////////////////////////////////////////////////
 
        for (auto& it : m_scenes[m_pszMainSceneName]->GetRenderables()) {
            
            CBChangesEveryFrame cb = {
                .World = XMMatrixTranspose(it.second->GetWorldMatrix()),
                .OutputColor = it.second->GetOutputColor(),
                .HasNormalMap = it.second->HasNormalMap(),
            };
            m_immediateContext->UpdateSubresource(it.second->GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);

            UINT stride[2] = { sizeof(SimpleVertex), sizeof(NormalData) };
            UINT offset[2] = { 0, 0 };

            m_immediateContext->IASetInputLayout(it.second->GetVertexLayout().Get());

            ComPtr<ID3D11Buffer> tmp[2] =
            {
                it.second->GetVertexBuffer(),
                it.second->GetNormalBuffer(),
            };
            m_immediateContext->IASetVertexBuffers(0, 2u, tmp->GetAddressOf(), stride, offset);
            m_immediateContext->IASetIndexBuffer(
                it.second->GetIndexBuffer().Get(),
                DXGI_FORMAT_R16_UINT, 0);

            m_immediateContext->VSSetShader(it.second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, it.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());


            m_immediateContext->VSSetShaderResources(2u,1u,m_shadowMapTexture->GetShaderResourceView().GetAddressOf());
            m_immediateContext->VSSetSamplers(2u,1u,m_shadowMapTexture->GetSamplerState().GetAddressOf());

            m_immediateContext->PSSetShaderResources(2u,1u,m_shadowMapTexture->GetShaderResourceView().GetAddressOf());
             m_immediateContext->PSSetSamplers(2u,1u,m_shadowMapTexture->GetSamplerState().GetAddressOf());


            m_immediateContext->PSSetShaderResources(3u,1u,m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(0)->pDiffuse->GetTextureResourceView().GetAddressOf());
            m_immediateContext->VSSetShaderResources(3u,1u, m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(0)->pDiffuse->GetTextureResourceView().GetAddressOf());


            m_immediateContext->PSSetShader(it.second->GetPixelShader().Get(), nullptr, 0);
            m_immediateContext->PSSetConstantBuffers(0u, 1u, it.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

            if (it.second->HasTexture())
            {
                for (UINT i = 0u; i < it.second->GetNumMeshes(); ++i)
                {
                    const UINT materialIndex = it.second->GetMesh(i).uMaterialIndex;
                    if (it.second->GetMaterial(materialIndex)->pDiffuse)
                    {
                        ComPtr<ID3D11ShaderResourceView> shaderResources =
                            it.second->GetMaterial(materialIndex)->pDiffuse->GetTextureResourceView();
                        ComPtr<ID3D11SamplerState> samplerStates =
                            it.second->GetMaterial(materialIndex)->pDiffuse->GetSamplerState();
                        m_immediateContext->PSSetShaderResources(0, 1, shaderResources.GetAddressOf());
                        ////////////////////////////////////////
                        Texture::eTextureSamplerType textureSamplerType = it.second->GetMaterial(materialIndex)->pDiffuse->GetSamplerType();
                        m_immediateContext->PSSetSamplers(
                            0u,
                            1u,
                            Texture::s_samplers[static_cast <size_t>(textureSamplerType)].GetAddressOf());
                        ////////////////////////////////////////

                    }
                    if (it.second->GetMaterial(materialIndex)->pNormal) {
                        ComPtr<ID3D11ShaderResourceView> shaderResources =
                            it.second->GetMaterial(materialIndex)->pNormal->GetTextureResourceView();
                        ComPtr<ID3D11SamplerState> samplerStates =
                            it.second->GetMaterial(materialIndex)->pNormal->GetSamplerState();
                        m_immediateContext->PSSetShaderResources(1, 1, shaderResources.GetAddressOf());
                        m_immediateContext->PSSetSamplers(1, 1, samplerStates.GetAddressOf());
                    }
                    m_immediateContext->DrawIndexed(
                        it.second->GetMesh(i).uNumIndices,
                        it.second->GetMesh(i).uBaseIndex,
                        it.second->GetMesh(i).uBaseVertex);
                }
            }
            else
            {
                m_immediateContext->DrawIndexed(it.second->GetNumIndices(), 0u, 0);
            }

        }
        //VOXEL RENDERING, INSTANCED
        for (auto& it : m_scenes[m_pszMainSceneName]->GetVoxels()) {
            UINT strides[3] = { 
                static_cast<UINT>(sizeof(SimpleVertex)), 
                static_cast<UINT>(sizeof(NormalData)),
                static_cast<UINT>(sizeof(InstanceData)) };
            UINT offsets[3] = { 0, 0,0 };

            ComPtr<ID3D11Buffer> vertexNormalInstanceBuffers[3] = {
                it->GetVertexBuffer(),
                it->GetNormalBuffer(),
                it->GetInstanceBuffer()
            };

            m_immediateContext->IASetVertexBuffers(0, 3, vertexNormalInstanceBuffers->GetAddressOf(), strides, offsets);

            m_immediateContext->IASetIndexBuffer(it->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(it->GetVertexLayout().Get());
            CBChangesEveryFrame cb = {
                .World = XMMatrixTranspose(it->GetWorldMatrix()),
                .OutputColor = it->GetOutputColor(),
                .HasNormalMap = it->HasNormalMap()
            };
            m_immediateContext->UpdateSubresource(
                it->GetConstantBuffer().Get(),
                0,
                nullptr,
                &cb,
                0,
                0
            );
            m_immediateContext->VSSetShader(it->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(2, 1, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(0, 1, it->GetConstantBuffer().GetAddressOf());
            //Now, VertexShader needs Lights constant
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());


            m_immediateContext->VSSetShaderResources(
                2u,
                1u,
                m_shadowMapTexture->GetShaderResourceView().GetAddressOf());

            m_immediateContext->VSSetSamplers(
                2u,
                1u,
                m_shadowMapTexture->GetSamplerState().GetAddressOf());


            m_immediateContext->PSSetShaderResources(
                2u,
                1u,
                m_shadowMapTexture->GetShaderResourceView().GetAddressOf());

            m_immediateContext->PSSetSamplers(
                2u,
                1u,
                m_shadowMapTexture->GetSamplerState().GetAddressOf());

            m_immediateContext->PSSetShaderResources(
                3u,
                1u,
                m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(0)->pDiffuse->GetTextureResourceView().GetAddressOf());


            m_immediateContext->VSSetShaderResources(
                3u,
                1u,
                m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(0)->pDiffuse->GetTextureResourceView().GetAddressOf());

            m_immediateContext->PSSetConstantBuffers(2, 1, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(0, 1, it->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());
            m_immediateContext->PSSetShader(it->GetPixelShader().Get(), nullptr, 0);


            if(it->HasTexture())
            {
                ComPtr<ID3D11ShaderResourceView> shaderResources[2] = { it->GetMaterial(0)->pDiffuse->GetTextureResourceView(),
                                                it->GetMaterial(0)->pNormal->GetTextureResourceView() };
                ComPtr<ID3D11SamplerState> samplerStates[2] = { it->GetMaterial(0)->pDiffuse->GetSamplerState(),
                                                it->GetMaterial(0)->pNormal->GetSamplerState() };
                m_immediateContext->PSSetShaderResources(0, 2, shaderResources->GetAddressOf());


                m_immediateContext->PSSetSamplers(0, 2, samplerStates->GetAddressOf());

                ////////////////////////////////////////나중에 위에꺼 지우자
                Texture::eTextureSamplerType textureSamplerType = it->GetMaterial(0)->pDiffuse->GetSamplerType();
                m_immediateContext->PSSetSamplers(
                    0u,
                    1u,
                    Texture::s_samplers[static_cast <size_t>(textureSamplerType)].GetAddressOf());
                ////////////////////////////////////////

                m_immediateContext->DrawIndexedInstanced(it->GetNumIndices(), it->GetNumInstances(), 0, 0, 0);

            }
            else
            {
                m_immediateContext->DrawIndexedInstanced(it->GetNumIndices(), it->GetNumInstances(), 0, 0, 0);
            }
        }
        //MODEL RENDERING
        //MODEL RENDERING
        //MODEL RENDERING
        //MODEL RENDERING
        //MODEL RENDERING
        //MODEL RENDERING
        //MODEL RENDERING
        for (auto& it : m_scenes[m_pszMainSceneName]->GetModels()) {
            UINT aStrides[3] = { 
                static_cast<UINT>(sizeof(SimpleVertex)),
                static_cast<UINT>(sizeof(NormalData)), 
                static_cast<UINT>(sizeof(AnimationData)) };
            UINT aOffsets[3] = { 0u, 0u ,0u};
            ComPtr<ID3D11Buffer> aBuffers[3] = { 
                it.second->GetVertexBuffer(),
                it.second->GetNormalBuffer(),
                it.second->GetAnimationBuffer() 
            };

            m_immediateContext->IASetVertexBuffers(0, 2, aBuffers->GetAddressOf(), aStrides, aOffsets);
            m_immediateContext->IASetIndexBuffer(it.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(it.second->GetVertexLayout().Get());
            CBChangesEveryFrame cbChangeEveryFrame = {
                .World = XMMatrixTranspose(it.second->GetWorldMatrix()),
                .OutputColor = it.second->GetOutputColor(),
                .HasNormalMap = it.second->HasNormalMap()
            };
            m_immediateContext->UpdateSubresource(
                it.second->GetConstantBuffer().Get(),
                0,
                nullptr,
                &cbChangeEveryFrame,
                0,
                0
            );
            CBSkinning cbSkinning = {
                .BoneTransforms = {}
            };
            for (UINT i = 0; i < it.second->GetBoneTransforms().size(); i++)
            {
                cbSkinning.BoneTransforms[i] = XMMatrixTranspose(it.second->GetBoneTransforms()[i]);
            }
            m_immediateContext->UpdateSubresource(
                it.second->GetSkinningConstantBuffer().Get(),
                0,
                nullptr,
                &cbSkinning,
                0,
                0
            );
            m_immediateContext->VSSetConstantBuffers(2, 1, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(0, 1, it.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(4, 1, it.second->GetSkinningConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetShader(it.second->GetVertexShader().Get(), nullptr, 0);
            //Now, VertexShader needs Lights constant
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

            //ON s2 and t2
            m_immediateContext->VSSetShaderResources(
                2u, 
                1u, 
                m_shadowMapTexture->GetShaderResourceView().GetAddressOf());

            m_immediateContext->VSSetSamplers(
                2u,
                1u,
                m_shadowMapTexture->GetSamplerState().GetAddressOf());


            m_immediateContext->PSSetShaderResources(
                2u,
                1u,
                m_shadowMapTexture->GetShaderResourceView().GetAddressOf());

            m_immediateContext->PSSetSamplers(
                2u,
                1u,
                m_shadowMapTexture->GetSamplerState().GetAddressOf());
;            


            m_immediateContext->PSSetShaderResources(
                3u,
                1u,
                m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(0)->pDiffuse->GetTextureResourceView().GetAddressOf());


            m_immediateContext->VSSetShaderResources(
                3u,
                1u,
                m_scenes[m_pszMainSceneName]->GetSkyBox()->GetMaterial(0)->pDiffuse->GetTextureResourceView().GetAddressOf());



            m_immediateContext->PSSetConstantBuffers(2, 1, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(0, 1, it.second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());
            m_immediateContext->PSSetShader(it.second->GetPixelShader().Get(), nullptr, 0);
            if (it.second->HasTexture())
            {
                for (UINT i = 0; i < it.second->GetNumMeshes(); i++)
                {
                    UINT materialIndex = it.second->GetMesh(i).uMaterialIndex;
                    ComPtr<ID3D11ShaderResourceView> shaderResources[2] = { it.second->GetMaterial(materialIndex)->pDiffuse->GetTextureResourceView() ,
                                                    it.second->GetMaterial(materialIndex)->pNormal->GetTextureResourceView() };
                    ComPtr<ID3D11SamplerState> samplerStates[2] = { it.second->GetMaterial(materialIndex)->pDiffuse->GetSamplerState() ,
                                                    it.second->GetMaterial(materialIndex)->pNormal->GetSamplerState() };
                    m_immediateContext->PSSetShaderResources(0, 2, shaderResources->GetAddressOf());
                    m_immediateContext->PSSetSamplers(0, 2, samplerStates->GetAddressOf());

                    ////////////////////////////////////////나중에 위에꺼 지우자
                    Texture::eTextureSamplerType textureSamplerType = it.second->GetMaterial(materialIndex)->pDiffuse->GetSamplerType();
                    m_immediateContext->PSSetSamplers(
                        0u,
                        1u,
                        Texture::s_samplers[static_cast <size_t>(textureSamplerType)].GetAddressOf());
                    ////////////////////////////////////////


                    m_immediateContext->DrawIndexed(it.second->GetMesh(i).uNumIndices, it.second->GetMesh(i).uBaseIndex, it.second->GetMesh(i).uBaseVertex);
                }
            }
            else
            {
                m_immediateContext->DrawIndexed(it.second->GetNumIndices(), 0, 0);
            }
        }
        m_swapChain->Present(0, 0);
        }

    


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetDriverType
      Summary:  Returns the Direct3D driver type
      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const
    {
        return m_driverType;
    }
}