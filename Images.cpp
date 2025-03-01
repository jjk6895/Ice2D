#include "pch.h"

#include "Images.h"
#include "SafeRelease.h"
#include "HRException.h"

namespace Ice2D
{
    unsigned int IBasicImage::GetWidth() const
    {
        return m_width;
    }

    unsigned int IBasicImage::GetHeight() const
    {
        return m_height;
    }

    IBasicImage::IBasicImage() : m_width(0u), m_height(0u)
    {
    }

    IBasicImage::IBasicImage(ResourceManager* pManager, unsigned int width, unsigned int height) :
        IBasicResource(pManager), m_width(width), m_height(height)
    {
    }

    IBasicImage::IBasicImage(ResourceManager* pManager) :
        IBasicResource(pManager), m_width(0u), m_height(0u)
    {
    }

    IBasicImage::IBasicImage(const IBasicImage& other) :
        IBasicResource(other), m_width(other.m_width), m_height(other.m_height)
    {
    }

    IBasicImage::~IBasicImage()
    {
    }

    IWICBitmapSource* IBasicImage::GetSourceFromFile(const wchar_t* path)
    {
        // Load the image using WIC
        IWICBitmapDecoder* pDecoder = nullptr;
        HRESULT hr = m_pManager->GetWICFactory()->CreateDecoderFromFilename(path, nullptr,
            GENERIC_READ, WICDecodeOptions::WICDecodeMetadataCacheOnDemand, &pDecoder);
        CheckHR(hr);

        // Get the first frame of the image
        IWICBitmapFrameDecode* pFrame = nullptr;
        hr = pDecoder->GetFrame(0, &pFrame);
        CheckHR(hr);

        // Convert the image to a Direct2D-compatible pixel format
        IWICFormatConverter* pConverter = nullptr;
        hr = m_pManager->GetWICFactory()->CreateFormatConverter(&pConverter);
        CheckHR(hr);

        // Initialize the format converter
        hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeErrorDiffusion, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
        CheckHR(hr);

        SafeRelease(pFrame);
        SafeRelease(pDecoder);
        return pConverter;
    }

    D2DImage::D2DImage() : m_pBitmap(nullptr)
    {
    }

    D2DImage::D2DImage(ResourceManager* pManager, unsigned int width, unsigned int height) :
        IBasicImage(pManager, width, height)
    {
        D2D1_PIXEL_FORMAT pixelFormat =
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
        HRESULT hr = pManager->GetRenderTarget()->CreateBitmap(D2D1::SizeU(width, height),
            D2D1::BitmapProperties(pixelFormat), &m_pBitmap);
        CheckHR(hr);
        OnLoad();
    }

    D2DImage::D2DImage(ResourceManager* pManager, const wchar_t* path) :
        IBasicImage(pManager)
    {
        // Create Direct2D bitmap from the converted WIC image
        auto pSource = GetSourceFromFile(path);
        D2D1_BITMAP_PROPERTIES bitmapProperties = {};
        bitmapProperties.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
        HRESULT hr = m_pManager->GetRenderTarget()->CreateBitmapFromWicBitmap(pSource, &bitmapProperties, &m_pBitmap);
        CheckHR(hr);
        SafeRelease(pSource);

        // Get size
        auto size = m_pBitmap->GetPixelSize();
        m_width = size.width;
        m_height = size.height;

        OnLoad();
    }

    D2DImage::D2DImage(const RawImage& other) :
        IBasicImage(other)
    {
        D2D1_BITMAP_PROPERTIES bitmapProperties = {};
        bitmapProperties.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
        HRESULT hr = m_pManager->GetRenderTarget()->CreateBitmapFromWicBitmap(other.m_pBitmap, 
            &bitmapProperties, &m_pBitmap);
        CheckHR(hr);

        OnLoad();
    }

    D2DImage::D2DImage(D2DImage&& other) noexcept : IBasicImage(other), m_pBitmap(other.m_pBitmap)
    {
        other.m_pBitmap = nullptr;
        OnLoad();
    }

    D2DImage& D2DImage::operator=(D2DImage&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pBitmap = other.m_pBitmap;
        other.m_pBitmap = nullptr;

        m_width = other.m_width;
        m_height = other.m_height;

        OnLoad();
        return *this;
    }

    D2DImage::~D2DImage()
    {
        Release();
    }

    void D2DImage::Release()
    {
        SafeRelease(m_pBitmap);
        OnUnload();
    }

    void D2DImage::CopyRaw(RawImage& other)
    {
        if (!m_pBitmap) throw std::runtime_error("D2D bitmap is null.");
        if (!other.m_pBitmap)
        {
            throw std::runtime_error("Raw bitmap is null in copy.");
        }

        if (m_width != other.m_width || m_height != other.m_height)
        {
            throw std::runtime_error("Image dimensions do not match in copy.");
        }

        bool wasLocked = other.IsLocked();
        other.Lock();
        HRESULT hr = m_pBitmap->CopyFromMemory(nullptr, other.m_pData, other.m_stride);
        CheckHR(hr);
        if (!wasLocked) other.Unlock();
    }

    ID2D1Bitmap* D2DImage::Get() const
    {
        if (!m_pBitmap) throw std::runtime_error("D2D bitmap is null.");
        return m_pBitmap;
    }

    RawImage::RawImage() : m_pBitmap(nullptr), m_pLock(nullptr), m_pData(nullptr), m_stride(0u), m_bufferSize(0u),
		m_pRT(nullptr)
    {
    }

    RawImage::RawImage(ResourceManager* pManager, unsigned int width, unsigned int height) :
        IBasicImage(pManager, width, height), m_pLock(nullptr), m_pData(nullptr), m_stride(0u), m_bufferSize(0u),
        m_pRT(nullptr)
    {
        HRESULT hr = pManager->GetWICFactory()->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA,
            WICBitmapCacheOnDemand, &m_pBitmap);
        CheckHR(hr);
        OnLoad();
    }

    RawImage::RawImage(ResourceManager* pManager, const wchar_t* path) :
        IBasicImage(pManager), m_pLock(nullptr), m_pData(nullptr), m_stride(0u), m_bufferSize(0u),
        m_pRT(nullptr)
    {
        // Copy from source into bitmap
        auto pSource = GetSourceFromFile(path);
        HRESULT hr = m_pManager->GetWICFactory()->CreateBitmapFromSource(pSource, WICBitmapCacheOnLoad, &m_pBitmap);
        CheckHR(hr);
        SafeRelease(pSource);

        // Get size
        hr = m_pBitmap->GetSize(&m_width, &m_height);
        CheckHR(hr);
        OnLoad();
    }

    RawImage::RawImage(RawImage&& other) noexcept : IBasicImage(other), m_pBitmap(other.m_pBitmap),
		m_pLock(nullptr), m_pData(nullptr), m_stride(0u), m_bufferSize(0u), m_pRT(other.m_pRT)
    {
        other.Unlock();
        other.m_pBitmap = nullptr;
        other.m_pRT = nullptr;
        OnLoad();
    }

    RawImage& RawImage::operator=(RawImage&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        other.Unlock();
		m_pRT = other.m_pRT;
        other.m_pRT = nullptr;
        m_pBitmap = other.m_pBitmap;
        other.m_pBitmap = nullptr;

        m_width = other.m_width;
        m_height = other.m_height;

        OnLoad();
        return *this;
    }

    RawImage::~RawImage()
    {
        Release();
    }

    void RawImage::Release()
    {
        Unlock();
		SafeRelease(m_pRT);
        SafeRelease(m_pBitmap);
        OnUnload();
    }

    void RawImage::CopyFrom(const RawImage& other)
    {
        if (!m_pBitmap) throw std::runtime_error("WIC bitmap is null.");
        if (!other.m_pBitmap)
        {
            throw std::runtime_error("Source bitmap is null in copy.");
        }

        if (m_bufferSize != other.m_bufferSize || m_width != other.m_width || m_height != other.m_height)
        {
            throw std::runtime_error("Image dimensions do not match in copy.");
        }

        bool wasLocked = IsLocked();
        Lock();
        HRESULT hr = other.m_pBitmap->CopyPixels(NULL, m_stride, m_bufferSize, 
            reinterpret_cast<BYTE*>(m_pData));
        CheckHR(hr);
        if (!wasLocked) Unlock();
        OnLoad();
    }

    void RawImage::Lock()
    {
        if (!m_pBitmap) throw std::runtime_error("WIC bitmap is null.");
        if (IsLocked()) return;
        WICRect rect = {};
        rect.Width = m_width;
        rect.Height = m_height;

        HRESULT hr = m_pBitmap->Lock(&rect, WICBitmapLockRead | WICBitmapLockWrite, &m_pLock);
        CheckHR(hr);

        BYTE* data;
        hr = m_pLock->GetDataPointer(&m_bufferSize, &data);
        CheckHR(hr);
        m_pData = reinterpret_cast<PixelColor*>(data);

        hr = m_pLock->GetStride(&m_stride);
        CheckHR(hr);

        unsigned int expectedSize = m_stride * m_height;
        if (m_bufferSize != expectedSize)
        {
            throw std::runtime_error("Buffer size mismatchunexpected memory layout");
        }
    }

    void RawImage::Unlock()
    {
        m_pData = nullptr;
		m_stride = 0u;
		m_bufferSize = 0u;
        SafeRelease(m_pLock);
    }

    bool RawImage::IsLocked() const
    {
        return m_pLock != nullptr;
    }

    RawImage::PixelColor RawImage::GetColor(unsigned int x, unsigned int y) const
    {
        if (!m_pBitmap) throw std::runtime_error("WIC bitmap is null.");
        if (!IsLocked()) throw std::runtime_error("Raw image is not locked.");
        if (x >= 0 && x < m_width && y >= 0 && y < m_height)
        {
            BYTE* rowStart = reinterpret_cast<BYTE*>(m_pData) + y * m_stride;

            return reinterpret_cast<PixelColor*>(rowStart)[x];
        }
        else return 0u;
    }

    void RawImage::SetColor(unsigned int x, unsigned int y, const PixelColor& c)
    {
        if (!m_pBitmap) throw std::runtime_error("WIC bitmap is null.");
        if (!IsLocked()) throw std::runtime_error("Raw image is not locked");
        if (x >= 0 && x < m_width && y >= 0 && y < m_height)
        {
            BYTE* rowStart = reinterpret_cast<BYTE*>(m_pData) + y * m_stride;

            reinterpret_cast<PixelColor*>(rowStart)[x] = c;
        }
    }

    void RawImage::ForEach(void (*process)(unsigned int x, unsigned int y, PixelColor& c, 
        void* pExtra, unsigned int extraSize), void* pExtra, unsigned int extraSize)
    {
        if (!m_pBitmap) throw std::runtime_error("WIC bitmap is null.");
        if (!IsLocked()) throw std::runtime_error("Raw image is not locked");

        for (unsigned int y = 0; y < m_height; ++y)
        {
            BYTE* rowStart = reinterpret_cast<BYTE*>(m_pData) + y * m_stride;

            for (unsigned int x = 0; x < m_width; ++x)
            {
                process(x, y, reinterpret_cast<PixelColor*>(rowStart)[x], pExtra, extraSize);
            }
        }
    }

    void RawImage::ForEach(void(*process)(unsigned int x, unsigned int y, PixelColor& c))
    {
        if (!m_pBitmap) throw std::runtime_error("WIC bitmap is null.");
        if (!IsLocked()) throw std::runtime_error("Raw image is not locked");

        for (unsigned int y = 0; y < m_height; ++y)
        {
            BYTE* rowStart = reinterpret_cast<BYTE*>(m_pData) + y * m_stride;

            for (unsigned int x = 0; x < m_width; ++x)
            {
                process(x, y, reinterpret_cast<PixelColor*>(rowStart)[x]);
            }
        }
    }

    ID2D1RenderTarget* RawImage::GetRenderTarget()
    {
		if (!m_pBitmap) throw std::runtime_error("WIC bitmap is null.");
        if (!m_pRT)
        {
			HRESULT hr = m_pManager->GetD2DFactory()->CreateWicBitmapRenderTarget(
                m_pBitmap,
				D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_DEFAULT, 
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                ), &m_pRT
            );
            CheckHR(hr);
        }
		return m_pRT;
    }

    void RawImage::SetAll(const PixelColor& c)
    {
		if (!m_pBitmap) throw std::runtime_error("WIC bitmap is null.");
		if (!IsLocked()) throw std::runtime_error("Raw image is not locked");
        for (unsigned int y = 0; y < m_height; ++y)
        {
            BYTE* rowStart = reinterpret_cast<BYTE*>(m_pData) + y * m_stride;

            for (unsigned int x = 0; x < m_width; ++x)
            {
                reinterpret_cast<PixelColor*>(rowStart)[x] = c;
            }
        }
    }

    RawImage::PixelColor::PixelColor() : data(0xFF000000)
    {
    }

    RawImage::PixelColor::PixelColor(UINT32 data) : data(data)
    {
    }

    RawImage::PixelColor::PixelColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a)
    {
        data = 0u;
        data |= a << 24;
        data |= r << 16;
        data |= g << 8;
        data |= b;
    }

    RawImage::PixelColor::PixelColor(const D2D1_COLOR_F& color) : 
        PixelColor(color.r * 255, color.g * 255, color.b * 255, color.a * 255)
    {
    }

    UINT8 RawImage::PixelColor::Red() const
    {
        return ((data >> 16) & 0xFF);
    }

    UINT8 RawImage::PixelColor::Green() const
    {
        return ((data >> 8) & 0xFF);
    }

    UINT8 RawImage::PixelColor::Blue() const
    {
        return (data & 0xFF);
    }

    UINT8 RawImage::PixelColor::Alpha() const
    {
        return ((data >> 24) & 0xFF);
    }

    void RawImage::PixelColor::SetRed(UINT8 r)
    {
        data = (data & ~(0xFF << 16)) | (r << 16);
    }

    void RawImage::PixelColor::SetGreen(UINT8 g)
    {
        data = (data & ~(0xFF << 8)) | (g << 8);
    }

    void RawImage::PixelColor::SetBlue(UINT8 b)
    {
        data = (data & ~(0xFF)) | b;
    }

    void RawImage::PixelColor::SetAlpha(UINT8 a)
    {
        data = (data & ~(0xFF << 24)) | (a << 24);
    }
	ImageRenderTarget::ImageRenderTarget() : m_pRT(nullptr)
	{
	}
    
    ImageRenderTarget::ImageRenderTarget(ResourceManager* pManager, unsigned int width, unsigned int height) :
		IBasicImage(pManager, width, height), m_pRT(nullptr)
	{
		HRESULT hr = pManager->GetRenderTarget()->CreateCompatibleRenderTarget(D2D1::SizeF(width, height),
			&m_pRT);
		CheckHR(hr);

		OnLoad();
	}

	ImageRenderTarget::ImageRenderTarget(ImageRenderTarget&& other) noexcept : IBasicImage(other), m_pRT(other.m_pRT)
	{
		other.m_pRT = nullptr;
		OnLoad();
	}

    ImageRenderTarget& ImageRenderTarget::operator=(ImageRenderTarget&& other) noexcept
    {
		if (this == &other) return *this;
		m_pManager = other.m_pManager;
		Release();
		m_pRT = other.m_pRT;
		other.m_pRT = nullptr;

		m_width = other.m_width;
		m_height = other.m_height;

		OnLoad();
		return *this;
    }

    ImageRenderTarget::~ImageRenderTarget()
    {
        Release();
    }

    void ImageRenderTarget::Release()
    {
        SafeRelease(m_pBitmap);
		SafeRelease(m_pRT);
		OnUnload();
    }

    ID2D1RenderTarget* ImageRenderTarget::GetRT()
    {
		if (!m_pRT) throw std::runtime_error("Render target is null.");
		return m_pRT;
    }

    ID2D1Bitmap* ImageRenderTarget::GetBitmap()
    {
		if (!m_pRT) throw std::runtime_error("Render target is null.");
        if (!m_pBitmap)
        {
			HRESULT hr = m_pRT->GetBitmap(&m_pBitmap);
			CheckHR(hr);
        }
		return m_pBitmap;
    }
}
