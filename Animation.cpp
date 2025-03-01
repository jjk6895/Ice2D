#include "pch.h"

#include "Images.h"
#include "SafeRelease.h"
#include "HRException.h"

namespace Ice2D
{
    IBasicAnimation::IBasicAnimation() : 
        m_frameCount(0u), m_currentFrame(0u), m_playOnce(false), m_playing(false), m_frameDelta(), m_startTime()
    {
    }

    IBasicAnimation::IBasicAnimation(ResourceManager* pManager, unsigned int frameCount, unsigned int frameRate) :
        IBasicImage(pManager), m_frameCount(frameCount), m_currentFrame(0u), m_playOnce(false), m_playing(false), m_startTime()
    {
        SetFrameRate(frameRate);
    }

    IBasicAnimation::IBasicAnimation(const IBasicAnimation& other) : IBasicImage(other),
		m_frameCount(other.m_frameCount), m_currentFrame(0u), m_playOnce(false), m_playing(false), m_startTime()
    {
        m_frameDelta = other.m_frameDelta;
    }

    void IBasicAnimation::Start(std::chrono::high_resolution_clock::time_point currentTime)
    {
        m_currentFrame = 0u;
        m_playing = true;
        m_startTime = currentTime;
    }

    void IBasicAnimation::Stop()
    {
        m_playing = false;
        m_playOnce = false;
        m_currentFrame = 0u;
    }

    void IBasicAnimation::Advance(std::chrono::high_resolution_clock::time_point currentTime)
    {
        if (m_playing)
        {
            auto elapsed = currentTime - m_startTime;
            auto frameNum = elapsed / m_frameDelta;
            m_currentFrame = frameNum % m_frameCount;
            if (m_playOnce && frameNum > m_frameCount)
            {
                Stop();
            }
        }
    }

    bool IBasicAnimation::IsPlaying()
    {
        return m_playing;
    }

    void IBasicAnimation::PlayOnce()
    {
        m_playOnce = true;
        Start();
    }

    void IBasicAnimation::SetFrameRate(unsigned int frameRate)
    {
        m_frameDelta = std::chrono::nanoseconds(1'000'000'000 / frameRate);
    }

    ImageSequence::ImageSequence() : m_pFrames(nullptr)
    {
    }

    ImageSequence::ImageSequence(ResourceManager* pManager, ID2D1Bitmap** arr_imgs, unsigned int frameCount, 
        unsigned int frameRate) : IBasicAnimation(pManager, frameCount, frameRate)
    {
		m_pFrames = new ID2D1Bitmap*[frameCount];
		for (unsigned int i = 0; i < frameCount; ++i)
		{
			auto bmp = arr_imgs[i];
			bmp->AddRef();
			m_pFrames[i] = bmp;
		}
		auto size = m_pFrames[0]->GetSize();
		m_width = (unsigned int)size.width;
		m_height = (unsigned int)size.height;

		OnLoad();
    }

    ImageSequence::ImageSequence(ResourceManager* pManager, const D2DImage* arr_imgs, unsigned int frameCount,
        unsigned int frameRate) : IBasicAnimation(pManager, frameCount, frameRate)
    {
        m_pFrames = new ID2D1Bitmap*[frameCount];
        for (unsigned int i = 0; i < frameCount; ++i)
        {
            auto bmp = arr_imgs[i].Get();
            bmp->AddRef();
            m_pFrames[i] = bmp;
        }

        auto size = m_pFrames[0]->GetSize();
        m_width = (unsigned int)size.width;
        m_height = (unsigned int)size.height;

        OnLoad();
    }

    ImageSequence::ImageSequence(ResourceManager* pManager, const wchar_t** arr_paths, unsigned int frameCount,
        unsigned int frameRate) : IBasicAnimation(pManager, frameCount, frameRate)
    {
        m_pFrames = new ID2D1Bitmap*[frameCount];
        for (unsigned int i = 0; i < frameCount; ++i)
        {
            auto pSource = GetSourceFromFile(arr_paths[i]);
            D2D1_BITMAP_PROPERTIES bitmapProperties = {};
            bitmapProperties.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
            HRESULT hr =
                m_pManager->GetRenderTarget()->CreateBitmapFromWicBitmap(pSource, &bitmapProperties, m_pFrames + i);
            CheckHR(hr);
            SafeRelease(pSource);
        }

        auto size = m_pFrames[0]->GetSize();
        m_width = (unsigned int)size.width;
        m_height = (unsigned int)size.height;
        
        OnLoad();
    }

    ImageSequence::ImageSequence(ImageSequence&& other) noexcept :
        IBasicAnimation(other), m_pFrames(other.m_pFrames)
    {
        other.m_pFrames = nullptr;
        OnLoad();
    }

    ImageSequence& ImageSequence::operator=(ImageSequence&& other) noexcept
    {
        if (&other == this) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pFrames = other.m_pFrames;
        other.m_pFrames = nullptr;

        m_width = other.m_width;
        m_height = other.m_height;

        m_frameCount = other.m_frameCount;
        m_frameDelta = other.m_frameDelta;
		m_startTime = other.m_startTime;
		m_frameDelta = other.m_frameDelta;
		m_currentFrame = other.m_currentFrame;

        OnLoad();
        return *this;
    }

    ImageSequence::~ImageSequence()
    {
        Release();
    }

    void ImageSequence::Release()
    {
        if (m_pFrames)
        {
            for (unsigned int i = 0; i < m_frameCount; ++i)
            {
                SafeRelease(m_pFrames[i]);
            }
            delete[] m_pFrames;
        }
        OnUnload();
    }

    ID2D1Bitmap* ImageSequence::Get()
    {
        if (!m_pFrames) throw std::runtime_error("Image sequence is null.");
        return m_pFrames[m_currentFrame];
    }

    AnimationSheet::AnimationSheet() : m_pSheet(nullptr), 
        m_rows(0u), m_cols(0u), m_spriteWidth(0u), m_spriteHeight(0u)
    {
    }

    AnimationSheet::AnimationSheet(ResourceManager* pManager, ID2D1Bitmap* pSheet,
        const unsigned int rows, const unsigned int cols, unsigned int frameCount, unsigned int frameRate) :
        IBasicAnimation(pManager, frameCount, frameRate), m_rows(rows), m_cols(cols)
    {
        pSheet->AddRef();
        m_pSheet = pSheet;

        auto size = pSheet->GetSize();
        m_width = (unsigned int)size.width;
        m_height = (unsigned int)size.height;
        m_spriteWidth = m_width / cols;
        m_spriteHeight = m_height / rows;

        OnLoad();
    }

    AnimationSheet::AnimationSheet(ResourceManager* pManager, const wchar_t* path,
        unsigned int rows, unsigned int cols, unsigned int frameCount, unsigned int frameRate) :
        IBasicAnimation(pManager, frameCount, frameRate), m_rows(rows), m_cols(cols)
    {
        auto pSource = GetSourceFromFile(path);
        D2D1_BITMAP_PROPERTIES bitmapProperties = {};
        bitmapProperties.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
        HRESULT hr =
            m_pManager->GetRenderTarget()->CreateBitmapFromWicBitmap(pSource, &bitmapProperties, &m_pSheet);
        CheckHR(hr);
        SafeRelease(pSource);

        auto size = m_pSheet->GetSize();
        m_width = (unsigned int)size.width;
        m_height = (unsigned int)size.height;
        m_spriteWidth = m_width / cols;
        m_spriteHeight = m_height / rows;

        OnLoad();
    }

    AnimationSheet::AnimationSheet(AnimationSheet&& other) noexcept : IBasicAnimation(other),
        m_pSheet(other.m_pSheet), m_rows(other.m_rows), m_cols(other.m_cols),
        m_spriteWidth(other.m_spriteWidth), m_spriteHeight(other.m_spriteHeight)
    {
        other.m_pSheet = nullptr;
        OnLoad();
    }

    AnimationSheet& AnimationSheet::operator=(AnimationSheet&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pSheet = other.m_pSheet;
        other.m_pSheet = nullptr;

        m_width = other.m_width;
        m_height = other.m_height;

        m_frameCount = other.m_frameCount;
        m_frameDelta = other.m_frameDelta;
        m_startTime = other.m_startTime;
        m_frameDelta = other.m_frameDelta;
        m_currentFrame = other.m_currentFrame;

        m_spriteWidth = other.m_spriteWidth;
        m_spriteHeight = other.m_spriteHeight;
        m_rows = other.m_rows;
        m_cols = other.m_cols;

        OnLoad();
        return *this;
    }

    AnimationSheet::~AnimationSheet()
    {
        Release();
    }

    void AnimationSheet::Release()
    {
        SafeRelease(m_pSheet);
        OnUnload();
    }

    ID2D1Bitmap* AnimationSheet::Get()
    {
        if (!m_pSheet) throw std::runtime_error("Animation sheet is null.");
        return m_pSheet;
    }

    D2D_RECT_F AnimationSheet::GetSourceRect()
    {
        unsigned int row = m_currentFrame / m_cols;
        unsigned int col = m_currentFrame % m_cols;

        D2D1_RECT_F frameRect;
        frameRect.left = (float)(col * m_spriteWidth);
        frameRect.top = (float)(row * m_spriteHeight);
        frameRect.right = frameRect.left + m_spriteWidth;
        frameRect.bottom = frameRect.top + m_spriteHeight;

        return frameRect;
    }
}