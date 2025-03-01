#pragma once
#include "ResourceManager.h"
#include "Graphics.h"
#include <chrono>

namespace Ice2D
{
	class Application : protected Graphics
	{
	public:
		Application(HINSTANCE hInstance,
			const unsigned int clientWidth, const unsigned int clientHeight,
			LPCWSTR title,
			const DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			const int nCmdShow = SW_SHOWNORMAL
		);
		Application(const Application& other) = delete;
		Application& operator=(const Application& other) = delete;
		~Application();
		int Start();
	protected:
		ResourceManager manager;
		virtual void Setup()  {}
		virtual HRESULT Draw() { return S_OK; }
		virtual void Update() {}
		std::chrono::high_resolution_clock::time_point currentTime;
		std::chrono::duration<float> deltaTime;
	};
}