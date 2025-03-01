#include "pch.h"

#include "Graphics.h"
#include "SafeRelease.h"
#include "HRException.h"

namespace Ice2D
{
    Graphics::Graphics(HINSTANCE hInstance, const unsigned int clientWidth, const unsigned int clientHeight,
        LPCWSTR title, const DWORD windowStyle, const int nCmdShow) :
        Window(hInstance, clientWidth, clientHeight, title, windowStyle, nCmdShow)
    {
        // Create d2d factory
        HRESULT hr;
        if (!m_pD2DFactory)
        {
#ifdef _DEBUG
            D2D1_FACTORY_OPTIONS options = {};
            options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
            hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &m_pD2DFactory);
#else
            hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
#endif
            CheckHR(hr);
        }

        // Create render target
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(m_clientWidth, m_clientHeight)),
            &m_pRenderTarget);
        CheckHR(hr);
    }

    Graphics::~Graphics()
    {
        try
        {
            HRESULT hr = m_pRenderTarget->EndDraw();
            if (FAILED(hr) && hr != D2DERR_WRONG_STATE)
            {
                throw HRException(hr);
            }
        }
        catch (const HRException& e)
        {
            HRException::ErrorBox(e);
        }
        SafeRelease(m_pRenderTarget);
        SafeRelease(m_pD2DFactory);
    }

    void Graphics::OnResize(const unsigned int width, const unsigned int height)
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        m_clientWidth = rc.right - rc.left;
        m_clientHeight = rc.bottom - rc.top;
        HRESULT hr = m_pRenderTarget->Resize(D2D1::SizeU(m_clientWidth, m_clientHeight));
        CheckHR(hr);
    }

    ID2D1Factory* Graphics::GetFactory() const
    {
        return m_pD2DFactory;
    }

    void Graphics::SetRotation(float angle, D2D_POINT_2F center)
    {
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(angle, center));
    }

    void Graphics::SetRotation(float angle, float center_x, float center_y)
    {
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(center_x, center_y)));
    }

    void Graphics::ClearTransform()
    {
        m_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
    }

    ID2D1HwndRenderTarget* Graphics::GetRT() const
    {
        return m_pRenderTarget;
    }
}