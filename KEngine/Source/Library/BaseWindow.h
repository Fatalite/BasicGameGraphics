#pragma once

#pragma once

#include "Common.h"

namespace library
{
    template <class DerivedType>
    class BaseWindow
    {
    public:
        static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

        BaseWindow();
        BaseWindow(const BaseWindow& rhs) = delete;
        BaseWindow(BaseWindow&& rhs) = delete;
        BaseWindow& operator=(const BaseWindow& rhs) = delete;
        BaseWindow& operator=(BaseWindow&& rhs) = delete;
        virtual ~BaseWindow() = default;

        virtual HRESULT Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName) = 0;
        virtual PCWSTR GetWindowClassName() const = 0;
        virtual LRESULT HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) = 0;

        HWND GetWindow() const;

    protected:
        HRESULT initialize(
            _In_ HINSTANCE hInstance,
            _In_ INT nCmdShow,
            _In_ PCWSTR pszWindowName,
            _In_ DWORD dwStyle,
            _In_opt_ INT x = CW_USEDEFAULT,
            _In_opt_ INT y = CW_USEDEFAULT,
            _In_opt_ INT nWidth = CW_USEDEFAULT,
            _In_opt_ INT nHeight = CW_USEDEFAULT,
            _In_opt_ HWND hWndParent = nullptr,
            _In_opt_ HMENU hMenu = nullptr
        );

        HINSTANCE m_hInstance;
        HWND m_hWnd;
        LPCWSTR m_pszWindowName;
    };


    template <class DerivedType>
    LRESULT CALLBACK BaseWindow<DerivedType>::WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
        //프로시저 => winmain의 subroutine
        //non-queue와 queue 메세지를 처리한다.
        DerivedType* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DerivedType*)pCreate->lpCreateParams;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hWnd = hWnd;
        }
        else
        {
            pThis = (DerivedType*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

    }

    template <class DerivedType>
    BaseWindow<DerivedType>::BaseWindow() : m_hInstance(NULL),m_hWnd(NULL),m_pszWindowName(NULL) {
    }

    template <class DerivedType>
    HWND BaseWindow<DerivedType>::GetWindow() const {
        //Wnd handler를 보낸다.
        return m_hWnd;
    }

    template <class DerivedType>
    HRESULT BaseWindow<DerivedType>::initialize(
        _In_ HINSTANCE hInstance,
        _In_ INT nCmdShow,
        _In_ PCWSTR pszWindowName,
        _In_ DWORD dwStyle,
        _In_opt_ INT x,
        _In_opt_ INT y,
        _In_opt_ INT nWidth,
        _In_opt_ INT nHeight,
        _In_opt_ HWND hWndParent,
        _In_opt_ HMENU hMenu
    ) {
        WNDCLASS wc = { 0 };

        wc.lpfnWndProc = DerivedType::WindowProc;
        wc.hInstance = hInstance;
        //CLASS NAME
        wc.lpszClassName = pszWindowName;
        wc.lpszMenuName = NULL;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpfnWndProc = WindowProc;
        
        struct WindowXY {
            INT x;
            INT y;
        };

        WindowXY xy_tmp;
        xy_tmp.x = 800;
        xy_tmp.y = 600;

        //RegisterClass(&wc);
        assert(SUCCEEDED(RegisterClass(&wc)));
        m_hWnd = CreateWindow(
            pszWindowName, L"Hello", WS_OVERLAPPEDWINDOW,
            xy_tmp.x, xy_tmp.y, nWidth, nHeight,
            NULL, NULL,hInstance, this
        );
        std::cout << "안녕";
        if (!m_hWnd)
            return E_FAIL;

        
        return S_OK;

        
    }
}