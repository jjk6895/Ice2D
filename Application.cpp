#include "pch.h"

#include "Application.h"
#include "HRException.h"

Ice2D::Application::Application(HINSTANCE hInstance, 
	const unsigned int clientWidth, const unsigned int clientHeight, 
	LPCWSTR title, const DWORD windowStyle, const int nCmdShow) : 
	Graphics(hInstance, clientWidth, clientHeight, title, windowStyle, nCmdShow),
	manager(GetRT()), deltaTime(), currentTime()
{
}

Ice2D::Application::~Application()
{
}

int Ice2D::Application::Start()
{
	try
	{
		Setup();
		auto prevTime = currentTime = std::chrono::high_resolution_clock::now();
		while (Ice2D::Window::HandleMessages())
		{
			currentTime = std::chrono::high_resolution_clock::now();
			deltaTime = currentTime - prevTime;
			prevTime = currentTime;

			Update();
			HRESULT hr = Draw();
			CheckHR(hr);
		}
	}
	catch (const HRException& e)
	{
		HRException::ErrorBox(e);
		return -1;
	}
	catch (const std::exception& e)
	{
		HRException::ErrorBox(e);
		return -1;
	}
	catch (...)
	{
		HRException::ErrorBox(L"Unknown Error", L"Cooked...");
		return -1;
	}

	return 0;
}
