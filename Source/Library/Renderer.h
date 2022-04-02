/*+===================================================================
  File:      RENDERER.H
  Summary:   Renderer header file contains declarations of Renderer
             class used for the lab samples of Game Graphics
             Programming course.
  Classes: Renderer
  © 2022 Kyung Hee University
===================================================================+*/
#pragma once

#include "Common.h"
using namespace Microsoft::WRL;
#include "MainWindow.h"

namespace library
{
    /*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
      Class:    Renderer
      Summary:  Renderer initializes Direct3D, and renders renderable
                data onto the screen
      Methods:  Initialize
                  Creates Direct3D device and swap chain
                Render
                  Renders the frame
                Renderer
                  Constructor.
                ~Renderer
                  Destructor.
    C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
    class Renderer final
    {
    public:
        Renderer();
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        Renderer& operator=(const Renderer& other) = delete;
        Renderer& operator=(Renderer&& other) = delete;
        ~Renderer() = default;

        HRESULT Initialize(_In_ HWND hWnd);
        void Render();
        //void Resize(UINT, UINT);

        HRESULT compileShaderFromFile(PCWSTR, PCSTR, PCSTR, ID3DBlob**);    private:
        //for inits
        D3D_DRIVER_TYPE m_driverType;
        D3D_FEATURE_LEVEL m_featureLevel;

        //---추가적인 변수-----//
        //D3D11_TEXTURE2D_DESC    m_bbDesc;
        //D3D11_VIEWPORT          m_viewport;
        //ComPtr<ID3D11Texture2D>         m_pDepthStencil;
        //ComPtr<ID3D11DepthStencilView>  m_pDepthStencilView;
        //ComPtr<ID3D11Texture2D> pBackBuffer;
        //----상관없음------//


        ComPtr<ID3D11Device> m_d3dDevice;
        ComPtr<ID3D11Device1> m_d3dDevice1;
        ComPtr<ID3D11DeviceContext> m_immediateContext;
        ComPtr<ID3D11DeviceContext1> m_immediateContext1;
        ComPtr<IDXGISwapChain> m_swapChain;
        ComPtr<IDXGISwapChain1> m_swapChain1;
        ComPtr<ID3D11RenderTargetView> m_renderTargetView;

        //for vertex and pixel shader
        ComPtr<ID3D11Buffer> m_vertextBuffer;
        ComPtr<ID3D11Buffer> m_indexBuffer;
        ComPtr<ID3D11InputLayout> m_vertexLayout;
        ComPtr<ID3D11PixelShader> m_pixelShader;
        ComPtr<ID3D11VertexShader> m_vertexShader;

    };
}