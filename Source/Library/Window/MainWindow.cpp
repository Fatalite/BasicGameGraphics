#include "MainWindow.h"

namespace library
{

    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName) 

    {


        HRESULT hr = S_OK;
        
        RECT rc = { 0, 0, 800, 600 };
        
        //AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        hr = initialize(hInstance, nCmdShow, pszWindowName, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr);
        
        if (FAILED(hr)) return hr;
        //for Clip Cursor!!!
        POINT p1, p2;
        p1.x = rc.left;
        p1.y = rc.top;
        p2.x = rc.right;
        p2.y = rc.bottom;

        ClientToScreen(m_hWnd, &p1);
        ClientToScreen(m_hWnd, &p2);

        rc.left = p1.x;
        rc.top = p1.y;
        rc.right = p2.x;
        rc.bottom = p2.y;

        ClipCursor(&rc);

        //RAWINPUTDEVICE
        RAWINPUTDEVICE rid = {
            .usUsagePage = 0x01,
            .usUsage = 0x02,
            .dwFlags = 0,
            .hwndTarget = NULL,
        };
        if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
            return E_FAIL;
        }


        return hr;
    };
    PCWSTR MainWindow::GetWindowClassName() const {
        return L"MainWindow";
    }
    LRESULT MainWindow::HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;
        



        switch (uMsg)
        {
        case WM_PAINT:
            hdc = BeginPaint(m_hWnd, &ps);
            EndPaint(m_hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        //테스트때문에 넣은 부분
        case WM_MOUSEMOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            std::string s = std::to_string(x);
            LPCWSTR tmpstr;
            tmpstr = (LPCWSTR)s.c_str();
            //OutputDebugStringW(tmpstr);
            return 0;
        }
        //WM_INPUT!!!! RawInput인듯
        case WM_INPUT:
        {
            UINT dataSize;
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), 
                RID_INPUT, 
                NULL, 
                &dataSize, 
                sizeof(RAWINPUTHEADER));
            if (dataSize > 0) {
                std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>(dataSize);
                if (GetRawInputData(
                    reinterpret_cast<HRAWINPUT>(lParam),
                    RID_INPUT,
                    rawData.get(),
                    &dataSize,
                    sizeof(RAWINPUTHEADER))==dataSize)
                {

                    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawData.get());
                    if (raw->header.dwType == RIM_TYPEMOUSE) {
                        m_mouseRelativeMovement.X = raw->data.mouse.lLastX;
                        if (raw->data.mouse.lLastX > -1) {
                           // OutputDebugStringW(L"안녕하세요?????");
                        }
                        //long a = (long)(raw->data.mouse.lLastX);
                        //std::string s = std::to_string(a);
                        //LPCWSTR tmpstr;
                        //tmpstr = (LPCWSTR)s.c_str();
                        //OutputDebugStringW(tmpstr);


                        m_mouseRelativeMovement.Y = raw->data.mouse.lLastY;
                        //a = (long)(m_mouseRelativeMovement.Y);
                        //s = std::to_string(a);
                        //tmpstr = (LPCWSTR)s.c_str();
                        //OutputDebugStringW(tmpstr);
                    };

                };
            }

            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }

        //키 내렸을 때
        case WM_KEYDOWN:
            switch (wParam) {
            case VK_LEFT:
                m_directions.bLeft = TRUE;
                break;
            case VK_RIGHT:
                m_directions.bRight = TRUE;
                break;
            case VK_UP:
                m_directions.bFront = TRUE;
                break;
            case VK_DOWN:
                m_directions.bBack = TRUE;
                break;
            case VK_SHIFT:
                m_directions.bDown = TRUE;
                break;
            case VK_SPACE:
                m_directions.bUp= TRUE;
                break;
                
            }
            return 0;
        //키 올렸을때
        case WM_KEYUP:
            switch (wParam) {
            case VK_LEFT:
                m_directions.bLeft = FALSE;
                break;
            case VK_RIGHT:
                m_directions.bRight = FALSE;
                break;
            case VK_UP:
                m_directions.bFront = FALSE;
                break;
            case VK_DOWN:
                m_directions.bBack = FALSE;
                break;
            case VK_SHIFT:
                m_directions.bDown = FALSE;
                break;
            case VK_SPACE:
                m_directions.bUp = FALSE;
                break;
            }
            return 0;
        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }

    const DirectionsInput& MainWindow::GetDirections() const {
        return m_directions;
    };
    const MouseRelativeMovement& MainWindow::GetMouseRelativeMovement() const {
        return m_mouseRelativeMovement;
    };
    void MainWindow::ResetMouseMovement() {
        m_mouseRelativeMovement.X = 0;
        m_mouseRelativeMovement.Y = 0;
    };

}