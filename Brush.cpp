#include "pch.h"

#include "Brush.h"
#include "SafeRelease.h"
#include "HRException.h"

namespace Ice2D
{
    SolidBrush::SolidBrush() : m_pBrush(nullptr)
    {
    }

    SolidBrush::SolidBrush(ResourceManager* pManager, const D2D1_COLOR_F& color) : IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateSolidColorBrush(color, &m_pBrush);
		CheckHR(hr);
        OnLoad();
    }

    SolidBrush::SolidBrush(ResourceManager* pManager, const float r, const float g, const float b, const float a) :
        IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(r, g, b, a), &m_pBrush);
		CheckHR(hr);
        OnLoad();
    }

    SolidBrush::SolidBrush(ResourceManager* pManager, const float brightness, const float a) :
        IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateSolidColorBrush(
            D2D1::ColorF(brightness, brightness, brightness, a), &m_pBrush
        );
		CheckHR(hr);
        OnLoad();
    }

    SolidBrush::SolidBrush(SolidBrush&& other) noexcept : IBasicResource(other), m_pBrush(other.m_pBrush)
    {
        other.m_pBrush = nullptr;
        OnLoad();
    }

    SolidBrush& SolidBrush::operator=(SolidBrush&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pBrush = other.m_pBrush;
        other.m_pBrush = nullptr;

        OnLoad();
        return *this;
    }

    SolidBrush::~SolidBrush()
    {
        Release();
    }

    void SolidBrush::Release()
    {
        SafeRelease(m_pBrush);
        OnUnload();
    }

    ID2D1SolidColorBrush* SolidBrush::Get() const
    {
        if (!m_pBrush) throw std::runtime_error("Solid brush is null.");
        return m_pBrush;
    }

    void SolidBrush::SetColor(const D2D1_COLOR_F& color)
    {
        if (!m_pBrush) throw std::runtime_error("Solid brush is null.");
        m_pBrush->SetColor(color);
    }

    void SolidBrush::SetColor(const float r, const float g, const float b, const float a)
    {
        if (!m_pBrush) throw std::runtime_error("Solid brush is null.");
        m_pBrush->SetColor(D2D1::ColorF(r, g, b, a));
    }

    void SolidBrush::SetColor(const float brightness, const float a)
    {
        if (!m_pBrush) throw std::runtime_error("Solid brush is null.");
        m_pBrush->SetColor(D2D1::ColorF(brightness, brightness, brightness, a));
    }

    void SolidBrush::SetColor(const SolidBrush& other)
    {
        if (!m_pBrush) throw std::runtime_error("Solid brush is null.");
        m_pBrush->SetColor(other.m_pBrush->GetColor());
    }

    BitmapBrush::BitmapBrush() : m_pBrush(nullptr)
    {
    }

    BitmapBrush::BitmapBrush(ResourceManager* pManager, ID2D1Bitmap* pBitmap) :
        IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateBitmapBrush(pBitmap, &m_pBrush);
        CheckHR(hr);
        OnLoad();
    }

    BitmapBrush::BitmapBrush(ResourceManager* pManager, ID2D1Bitmap* pBitmap,
        const D2D1_BITMAP_BRUSH_PROPERTIES& bitmapBrushProperties,
        const D2D1_BRUSH_PROPERTIES& brushProperties) : IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateBitmapBrush(pBitmap, bitmapBrushProperties,
            brushProperties, &m_pBrush);
        CheckHR(hr);
        OnLoad();
    }

    BitmapBrush::BitmapBrush(ResourceManager* pManager, ID2D1Bitmap* pBitmap, float offsetX, float offsetY) :
        BitmapBrush(pManager, pBitmap, D2D1::BitmapBrushProperties(), D2D1::BrushProperties(1.0f,
            D2D1::Matrix3x2F::Translation(D2D1::SizeF(offsetX, offsetY))))
    {
    }

    BitmapBrush::BitmapBrush(BitmapBrush&& other) noexcept : IBasicResource(other), m_pBrush(other.m_pBrush)
    {
        other.m_pBrush = nullptr;
        OnLoad();
    }

    BitmapBrush& BitmapBrush::operator=(BitmapBrush&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pBrush = other.m_pBrush;
        other.m_pBrush = nullptr;

        OnLoad();
        return *this;
    }

    BitmapBrush::~BitmapBrush()
    {
        Release();
    }

    void BitmapBrush::Release()
    {
        SafeRelease(m_pBrush);
        OnUnload();
    }

    void BitmapBrush::SetTranslation(float x, float y)
    {
        if (!m_pBrush) throw std::runtime_error("Bitmap brush is null.");
        m_pBrush->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(x, y)));
    }

    ID2D1BitmapBrush* BitmapBrush::Get() const
    {
        if (!m_pBrush) throw std::runtime_error("Bitmap brush is null.");
        return m_pBrush;
    }

    GradientStops::GradientStops() : m_pStops(nullptr)
    {
    }

    GradientStops::GradientStops(ResourceManager* pManager, unsigned int expectedCount) :
        IBasicResource(pManager), m_pStops(nullptr)
    {
        m_vecStops.reserve(expectedCount);
        OnLoad();
    }

    GradientStops::GradientStops(GradientStops&& other) noexcept : IBasicResource(other), m_pStops(nullptr)
    {
        m_vecStops = other.m_vecStops;
        OnLoad();
    }

    GradientStops& GradientStops::operator=(GradientStops&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_vecStops = other.m_vecStops;
        return *this;
    }

    GradientStops::~GradientStops()
    {
        Release();
    }

    void GradientStops::Release()
    {
        SafeRelease(m_pStops);
        OnUnload();
    }

    size_t GradientStops::StopCount() const
    {
		return m_vecStops.size();
    }

    void GradientStops::CopyFrom(const GradientStops& other)
    {
        m_vecStops = other.m_vecStops;
    }

    void GradientStops::AddStop(float pos, const D2D1_COLOR_F& color)
    {
        m_vecStops.emplace_back(D2D1::GradientStop(pos, color));
    }

    void GradientStops::Recreate()
    {
        if (!m_pManager) throw std::runtime_error("Gradient stops has no resource manager.");
        Release();
        if (m_vecStops.empty()) throw std::runtime_error("No gradient stops were added, failed to create.");
        HRESULT hr = m_pManager->GetRenderTarget()->CreateGradientStopCollection(
            m_vecStops.data(), (UINT32)m_vecStops.size(), &m_pStops);
        CheckHR(hr);
        OnLoad();
    }

    ID2D1GradientStopCollection* GradientStops::Get()
    {
        if (!m_pStops) Recreate();
        return m_pStops;
    }

    LinearBrush::LinearBrush() : m_pBrush(nullptr)
    {
    }

    LinearBrush::LinearBrush(ResourceManager* pManager, ID2D1GradientStopCollection* pStops,
        const D2D_POINT_2F& start, const D2D_POINT_2F& end) : IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(start, end),
            pStops, &m_pBrush
        );
        CheckHR(hr);
        OnLoad();
    }

    LinearBrush::LinearBrush(ResourceManager* pManager, ID2D1GradientStopCollection* pStops,
        const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& gradientProperies,
        const D2D1_BRUSH_PROPERTIES& brushProperties) : IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateLinearGradientBrush(
            gradientProperies, brushProperties, pStops, &m_pBrush
        );
        CheckHR(hr);
        OnLoad();
    }

    LinearBrush::LinearBrush(LinearBrush&& other) noexcept : 
        IBasicResource(other), m_pBrush(other.m_pBrush)
    {
        other.m_pBrush = nullptr;
        OnLoad();
    }

    LinearBrush& LinearBrush::operator=(LinearBrush&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pBrush = other.m_pBrush;
        other.m_pBrush = nullptr;

        OnLoad();
        return *this;
    }

    LinearBrush::~LinearBrush()
    {
        Release();
    }

    void LinearBrush::Release()
    {
        SafeRelease(m_pBrush);
        OnUnload();
    }

    ID2D1LinearGradientBrush* LinearBrush::Get()
    {
        if (!m_pBrush) throw std::runtime_error("Linear gradient brush is null.");
        return m_pBrush;
    }

    void LinearBrush::SetPos(const D2D_POINT_2F& start, const D2D_POINT_2F& end)
    {
        if (!m_pBrush) throw std::runtime_error("Linear gradient brush is null.");
        m_pBrush->SetStartPoint(start);
        m_pBrush->SetEndPoint(end);
    }

    RadialBrush::RadialBrush() : m_pBrush(nullptr)
    {
    }

    RadialBrush::RadialBrush(ResourceManager* pManager, ID2D1GradientStopCollection* pStops,
        const D2D1_POINT_2F& center, const D2D1_POINT_2F& originOffset, float radiusX, float radiusY) :
        IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(center, originOffset, radiusX, radiusY),
            pStops, &m_pBrush
        );
        CheckHR(hr);
        OnLoad();
    }

    RadialBrush::RadialBrush(ResourceManager* pManager, ID2D1GradientStopCollection* pStops,
        const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& gradientProperies, const D2D1_BRUSH_PROPERTIES& brushProperties) :
        IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetRenderTarget()->CreateRadialGradientBrush(
            gradientProperies, brushProperties, pStops, &m_pBrush
        );
        CheckHR(hr);
        OnLoad();
    }

    RadialBrush::RadialBrush(RadialBrush&& other) noexcept : 
        IBasicResource(other), m_pBrush(other.m_pBrush)
    {
        other.m_pBrush = nullptr;
        OnLoad();
    }

    RadialBrush& RadialBrush::operator=(RadialBrush&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pBrush = other.m_pBrush;
        other.m_pBrush = nullptr;

        OnLoad();
        return *this;
    }

    RadialBrush::~RadialBrush()
    {
        Release();
    }

    void RadialBrush::Release()
    {
        SafeRelease(m_pBrush);
        OnUnload();
    }

    ID2D1RadialGradientBrush* RadialBrush::Get()
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        return m_pBrush;
    }

    void RadialBrush::SetCenter(const D2D_POINT_2F& center)
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        m_pBrush->SetCenter(center);
    }

    D2D_POINT_2F RadialBrush::GetCenter()
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        return m_pBrush->GetCenter();
    }

    void RadialBrush::SetOffset(const D2D_POINT_2F& offset)
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        m_pBrush->SetGradientOriginOffset(offset);
    }

    D2D_POINT_2F RadialBrush::GetOffset()
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        return m_pBrush->GetGradientOriginOffset();
    }

    void RadialBrush::SetRadiusX(float x)
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        m_pBrush->SetRadiusX(x);
    }

    void RadialBrush::SetRadiusY(float y)
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        m_pBrush->SetRadiusY(y);
    }

    void RadialBrush::SetRadius(const D2D_POINT_2F& r)
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        m_pBrush->SetRadiusX(r.x);
        m_pBrush->SetRadiusY(r.y);
    }

    void RadialBrush::SetRadius(float x, float y)
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        m_pBrush->SetRadiusX(x);
        m_pBrush->SetRadiusY(y);
    }

    void RadialBrush::SetRadius(float r)
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        m_pBrush->SetRadiusX(r);
        m_pBrush->SetRadiusY(r);
    }

    float RadialBrush::GetRadiusX()
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        return m_pBrush->GetRadiusX();
    }

    float RadialBrush::GetRadiusY()
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        return m_pBrush->GetRadiusY();
    }

    D2D_POINT_2F RadialBrush::GetRadius()
    {
        if (!m_pBrush) throw std::runtime_error("Radial gradient brush is null.");
        return D2D1::Point2F(m_pBrush->GetRadiusX(), m_pBrush->GetRadiusY());
    }
}
