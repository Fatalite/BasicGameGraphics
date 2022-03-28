#include "MainWindow.h"

namespace library
{

    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName) 

    {
        HRESULT hr = S_OK;

        hr = this->initialize(hInstance,nCmdShow,GetWindowClassName(),NULL);
        
        return hr;
    };
    PCWSTR MainWindow::GetWindowClassName() const {
        return L"MainWindow";
    }
    LRESULT MainWindow::HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
        switch (uMsg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(this->GetWindow(), &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(this->GetWindow(), &ps);
            return 0;
        }
        
        //cas WM_SIZE
        default:
            return DefWindowProc(this->GetWindow(), uMsg, wParam, lParam);
        }
        return TRUE;
    }
}