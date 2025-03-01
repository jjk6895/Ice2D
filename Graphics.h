#pragma once
#include "Window.h"
#include <d2d1.h>

namespace Ice2D
{
    class Graphics : public Window
    {
    public:
        Graphics(HINSTANCE hInstance, const unsigned int clientWidth, const unsigned int clientHeight, LPCWSTR title,
            const DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            const int nCmdShow = SW_SHOWNORMAL);
        Graphics(const Graphics& other) = delete;
        void operator=(const Graphics& other) = delete;
        ~Graphics();
        ID2D1HwndRenderTarget* GetRT() const;
        ID2D1Factory* GetFactory() const;
        void SetRotation(float angle, D2D_POINT_2F center = D2D1::Point2F());
        void SetRotation(float angle, float center_x, float center_y);
        void ClearTransform();
    private:
        ID2D1Factory* m_pD2DFactory;
        ID2D1HwndRenderTarget* m_pRenderTarget;
        void OnResize(const unsigned int width, const unsigned int height) override;
    };
}