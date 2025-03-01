#pragma once

#include "MinWin.h"

namespace Ice2D
{
	class Window
	{
	public:
		Window(HINSTANCE hInstance, const unsigned int clientWidth, const unsigned int clientHeight, LPCWSTR title,
			const DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			const int nCmdShow = SW_SHOWNORMAL);
		~Window();
		static bool HandleMessages();
		unsigned int GetClientWidth() const;
		unsigned int GetClientHeight() const;
		HWND GetWindowHandle() const;
		struct
		{
			bool keyboardState[0xFF];
			bool mouseButtons[3];
			int mouseX, mouseY;
		} input;
		void ClearInput();
		void Quit();
	private:
		static constexpr LPCWSTR CLASS_NAME = L"Engine Window";
		static int nInstances;
		static LRESULT CALLBACK SetupWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		static HINSTANCE hInstance;
	protected:
		unsigned int m_clientWidth, m_clientHeight;
		HWND hwnd;
		virtual void OnResize(const unsigned int width, const unsigned int height);
	};
}