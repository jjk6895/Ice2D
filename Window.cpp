#include "pch.h"

#include "Window.h"

namespace Ice2D
{
	HINSTANCE Window::hInstance;
	Window::Window(HINSTANCE hInstance, const unsigned int clientWidth, const unsigned int clientHeight, LPCWSTR title,
		const DWORD windowStyle, const int nCmdShow) :
		m_clientWidth(clientWidth), m_clientHeight(clientHeight), hwnd(NULL)
	{
		ClearInput();

		// Register window class
		static bool classRegistered = false;
		if (!classRegistered)
		{
			WNDCLASS wc = {};
			wc.lpfnWndProc = SetupWindowProc;
			wc.hInstance = hInstance;
			wc.lpszClassName = CLASS_NAME;
			wc.cbWndExtra = sizeof(Window*);
			RegisterClass(&wc);
			classRegistered = true;
			Window::hInstance = hInstance;
		}

		// Adjust window rect
		RECT wr = { 0, 0, (LONG)clientWidth, (LONG)clientHeight };
		AdjustWindowRect(&wr, windowStyle, FALSE);

		// Create window
		CreateWindowEx(0, CLASS_NAME, title, windowStyle,
			CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
			NULL, NULL, hInstance, this);

		if (!hwnd)
		{
			MessageBox(NULL, L"Failed to create window.", L"Window was not created.", MB_ICONERROR | MB_OK);
			return;
		}

		ShowWindow(hwnd, nCmdShow);
	}

	Window::~Window()
	{
		Quit();
	}

	void Window::ClearInput()
	{
		for (int i = 0; i < 0xFF; ++i) input.keyboardState[i] = false;
		for (int i = 0; i < 3; ++i) input.mouseButtons[i] = false;
	}

	void Window::Quit()
	{
		SendMessage(hwnd, WM_DESTROY, 0, 0);
		HandleMessages();
	}

	int Window::nInstances = 0;
	LRESULT Window::SetupWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			Window* pThis = static_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::StaticWindowProc));
			++nInstances;
			pThis->hwnd = hwnd;
			return pThis->WindowProc(uMsg, wParam, lParam);
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	LRESULT Window::StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Window* pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (pThis) return pThis->WindowProc(uMsg, wParam, lParam);
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	LRESULT Window::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
			input.keyboardState[wParam] = true;
			break;
		case WM_KEYUP:
			input.keyboardState[wParam] = false;
			break;
		case WM_LBUTTONDOWN:
			input.mouseButtons[0] = true;
			break;
		case WM_LBUTTONUP:
			input.mouseButtons[0] = false;
			break;
		case WM_MBUTTONDOWN:
			input.mouseButtons[1] = true;
			break;
		case WM_MBUTTONUP:
			input.mouseButtons[1] = false;
			break;
		case WM_RBUTTONDOWN:
			input.mouseButtons[2] = true;
			break;
		case WM_RBUTTONUP:
			input.mouseButtons[2] = false;
			break;
		case WM_MOUSEMOVE:
			input.mouseX = LOWORD(lParam);
			input.mouseY = HIWORD(lParam);
			break;
		case WM_SIZE:
			OnResize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_DESTROY:
			--nInstances;
			if (nInstances < 1) PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);;
	}

	void Window::OnResize(unsigned int width, unsigned int height)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		m_clientWidth = rc.right - rc.left;
		m_clientHeight = rc.bottom - rc.top;
	}

	bool Window::HandleMessages()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				UnregisterClass(CLASS_NAME, hInstance);
				return false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}

	unsigned int Window::GetClientWidth() const
	{
		return m_clientWidth;
	}

	unsigned int Window::GetClientHeight() const
	{
		return m_clientHeight;
	}

	HWND Window::GetWindowHandle() const
	{
		return hwnd;
	}
}