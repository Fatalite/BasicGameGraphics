﻿#include "Game/Game.h"

namespace library
{
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Game::Game
	  Summary:  Constructor
	  Args:     PCWSTR pszGameName
				  Name of the game
	  Modifies: [m_pszGameName, m_mainWindow, m_renderer].
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	/*--------------------------------------------------------------------
	  TODO: Game::Game definition (remove the comment)
	--------------------------------------------------------------------*/
	Game::Game(_In_ PCWSTR pszGameName) {

		//게임 이름
		this->m_pszGameName = pszGameName;

		//mainWindow 인스턴스
		m_mainWindow = std::unique_ptr<MainWindow>(new MainWindow());
		m_renderer = std::unique_ptr<Renderer>(new Renderer());
		
	};
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Game::Initialize
	  Summary:  Initializes the components of the game
	  Args:     HINSTANCE hInstance
				  Handle to the instance
				INT nCmdShow
				  Is a flag that says whether the main application window
				  will be minimized, maximized, or shown normally
	  Modifies: [m_mainWindow, m_renderer].
	  Returns:  HRESULT
				Status code
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	/*--------------------------------------------------------------------
	  TODO: Game::Initialize definition (remove the comment)
	--------------------------------------------------------------------*/
	HRESULT Game::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow) {
		//mainWindow init
		HRESULT HR = m_mainWindow->Initialize(hInstance, nCmdShow, GetGameName());
		//assert(SUCCEEDED(HR));
		if (!SUCCEEDED(HR)) {
			//mainwindow가 init에 실패하면
			return HR;
		}
		//Renderer init
		HR = m_renderer->Initialize(m_mainWindow->GetWindow());
		//assert(SUCCEEDED(HR));

		return HR;
	};
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Game::Run
	  Summary:  Runs the game loop
	  Returns:  INT
				  Status code to return to the operating system
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	/*--------------------------------------------------------------------
	  TODO: Game::Run definition (remove the comment)
	--------------------------------------------------------------------*/
	INT Game::Run() {
		MSG msg = {};

		LARGE_INTEGER frequency;
		LARGE_INTEGER startTime;
		LARGE_INTEGER stopTime;
		float elapsedTime;
		QueryPerformanceCounter(&startTime);
		//FREQUENCY?????????????????????
		QueryPerformanceFrequency(&frequency);
		//주 창의 반복
		while (true)
		{

			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE) {
				if (msg.message == WM_QUIT) {
					break;
				}
				else {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else {
				//updata the elapsed time
				QueryPerformanceCounter(&stopTime);
				elapsedTime = (float)(stopTime.QuadPart - startTime.QuadPart);
				elapsedTime /= (float)(frequency.QuadPart);
				
				// WINDOW TO RENDERER INPUT INFORMATION
				m_renderer->HandleInput(
					m_mainWindow->GetDirections(),
					m_mainWindow->GetMouseRelativeMovement(),
					elapsedTime
				);
				m_mainWindow->ResetMouseMovement();


				//update game logic
				m_renderer->Update(elapsedTime);
				//render
				m_renderer->Render();
			}
			
			
		}

		return 0;
	}
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Game::GetGameName
	  Summary:  Returns the name of the game
	  Returns:  PCWSTR
				  Name of the game
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	/*--------------------------------------------------------------------
	  TODO: Game::GetGameName definition (remove the comment)
	--------------------------------------------------------------------*/
	PCWSTR Game::GetGameName() const {
		return m_pszGameName;
	}

	std::unique_ptr<MainWindow>& Game::GetWindow() {
		return m_mainWindow;
	};
	std::unique_ptr<Renderer>& Game::GetRenderer() {
		return m_renderer;
	};

}