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

        if (uMsg == WM_CREATE)
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
        WNDCLASS wc = { 
        .lpfnWndProc = DerivedType::WindowProc,
        .hInstance = GetModuleHandle(NULL),
        .lpszClassName = GetWindowClassName()
        };
        RegisterClass(&wc);
        //OutputDebugString((L"Test Output Debug\n"));
        m_hWnd = CreateWindowEx(
            NULL , GetWindowClassName(), pszWindowName, WS_OVERLAPPEDWINDOW, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
        );


        
        return S_OK;

        
    }
}