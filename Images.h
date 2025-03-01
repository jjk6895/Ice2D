#pragma once
#include "ResourceManager.h"
#include <d2d1.h>
#include <chrono>

namespace Ice2D
{
	class IBasicImage : protected IBasicResource
	{
	public:
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
	protected:
		IBasicImage();
		IBasicImage(ResourceManager* pManager, unsigned int width, unsigned int height);
		IBasicImage(ResourceManager* pManager);
		IBasicImage(const IBasicImage& other);
		IBasicImage& operator=(const IBasicImage& other) = delete;
		~IBasicImage();
		IWICBitmapSource* GetSourceFromFile(const wchar_t* path);
		unsigned int m_width, m_height;
	};

	class RawImage;
	class D2DImage : public IBasicImage
	{
	public:
		D2DImage();
		D2DImage(ResourceManager* pManager, unsigned int width, unsigned int height);
		D2DImage(ResourceManager* pManager, const wchar_t* path);
		D2DImage(const RawImage& other);
		D2DImage(const D2DImage& other) = delete;
		D2DImage& operator=(const D2DImage& other) = delete;
		D2DImage(D2DImage&& other) noexcept;
		D2DImage& operator=(D2DImage&& other) noexcept;
		~D2DImage();
		void Release() override;
		void CopyRaw(RawImage& other);
		ID2D1Bitmap* Get() const;
	private:
		ID2D1Bitmap* m_pBitmap;
	};

	class RawImage : public IBasicImage
	{
		friend class D2DImage;
	public:
		RawImage();
		RawImage(ResourceManager* pManager, unsigned int width, unsigned int height);
		RawImage(ResourceManager* pManager, const wchar_t* path);
		RawImage(const RawImage& other) = delete;
		RawImage& operator=(const RawImage& other) = delete;
		RawImage(RawImage&& other) noexcept;
		RawImage& operator=(RawImage&& other) noexcept;
		~RawImage();
		void Release() override;
		void CopyFrom(const RawImage& other);
		struct PixelColor
		{
			UINT32 data;
			PixelColor();
			PixelColor(UINT32 data);
			PixelColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a = 255u);
			PixelColor(const D2D1_COLOR_F& color);
			UINT8 Red() const;
			UINT8 Green() const;
			UINT8 Blue() const;
			UINT8 Alpha() const;
			void SetRed(UINT8 r);
			void SetGreen(UINT8 g);
			void SetBlue(UINT8 b);
			void SetAlpha(UINT8 a);
		};
		void Lock();
		void Unlock();
		bool IsLocked() const;
		PixelColor GetColor(unsigned int x, unsigned int y) const;
		void SetColor(unsigned int x, unsigned int y, const PixelColor& c);
		void ForEach(void (*process)(unsigned int x, unsigned int y, PixelColor& c, 
			void* pExtra, unsigned int extraSize), void* pExtra, unsigned int extraSize);
		void ForEach(void (*process)(unsigned int x, unsigned int y, PixelColor& c));
		ID2D1RenderTarget* GetRenderTarget();
		void SetAll(const PixelColor& c);
	private:
		IWICBitmap* m_pBitmap;
		IWICBitmapLock* m_pLock;
		PixelColor* m_pData;
		unsigned int m_bufferSize;
		unsigned int m_stride;
		ID2D1RenderTarget* m_pRT;
	};

	class IBasicAnimation : public IBasicImage
	{
	public:
		IBasicAnimation();
		IBasicAnimation(ResourceManager* pManager, unsigned int frameCount, unsigned int frameRate);
		IBasicAnimation(const IBasicAnimation& other);
		IBasicAnimation& operator=(const IBasicAnimation& other) = delete;
		void Start(std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now());
		void Stop();
		void Advance(std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now());
		bool IsPlaying();
		void PlayOnce();
		void SetFrameRate(unsigned int frameRate);
		virtual ID2D1Bitmap* Get() = 0;
	protected:
		unsigned int m_frameCount, m_currentFrame;
		std::chrono::high_resolution_clock::time_point m_startTime;
		std::chrono::high_resolution_clock::duration m_frameDelta;
		bool m_playing, m_playOnce;
	};

	class ImageSequence : public IBasicAnimation
	{
	public:
		ImageSequence();
		ImageSequence(ResourceManager* pManager, ID2D1Bitmap** arr_imgs, unsigned int frameCount,
			unsigned int frameRate);
		ImageSequence(ResourceManager* pManager, const D2DImage* arr_imgs, unsigned int frameCount, 
			unsigned int frameRate);
		ImageSequence(ResourceManager* pManager, const wchar_t** arr_paths, unsigned int frameCount, 
			unsigned int frameRate);
		ImageSequence(const ImageSequence& other) = delete;
		ImageSequence& operator=(const ImageSequence& other) = delete;
		ImageSequence(ImageSequence&& other) noexcept;
		ImageSequence& operator=(ImageSequence&& other) noexcept;
		~ImageSequence();
		void Release() override;
		ID2D1Bitmap* Get() override;
	private:
		ID2D1Bitmap** m_pFrames;
	};

	class AnimationSheet : public IBasicAnimation
	{
	public:
		AnimationSheet();
		AnimationSheet(ResourceManager* pManager, ID2D1Bitmap* pSheet,
			unsigned int rows, unsigned int cols, unsigned int frameCount, unsigned int frameRate);
		AnimationSheet(ResourceManager* pManager, const wchar_t* path,
			unsigned int rows, unsigned int cols, unsigned int frameCount, unsigned int frameRate);
		AnimationSheet(const AnimationSheet& other) = delete;
		AnimationSheet& operator=(const AnimationSheet& other) = delete;
		AnimationSheet(AnimationSheet&& other) noexcept;
		AnimationSheet& operator=(AnimationSheet&& other) noexcept;
		~AnimationSheet();
		void Release() override;
		ID2D1Bitmap* Get() override;
		D2D_RECT_F GetSourceRect();
	private:
		unsigned int m_rows, m_cols;
		unsigned int m_spriteWidth, m_spriteHeight;
		ID2D1Bitmap* m_pSheet;
	};

	class ImageRenderTarget : public IBasicImage
	{
	public:
		ImageRenderTarget();
		ImageRenderTarget(ResourceManager* pManager, unsigned int width, unsigned int height);
		ImageRenderTarget(const ImageRenderTarget& other) = delete;
		ImageRenderTarget& operator=(const ImageRenderTarget& other) = delete;
		ImageRenderTarget(ImageRenderTarget&& other) noexcept;
		ImageRenderTarget& operator=(ImageRenderTarget&& other) noexcept;
		~ImageRenderTarget();
		void Release() override;
		ID2D1RenderTarget* GetRT();
		ID2D1Bitmap* GetBitmap();
	private:
		ID2D1BitmapRenderTarget* m_pRT;
		ID2D1Bitmap* m_pBitmap;
	};
}