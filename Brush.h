#pragma once
#include "ResourceManager.h"
#include <d2d1.h>
#include <vector>

namespace Ice2D
{
	class SolidBrush : private IBasicResource
	{
	public:
		SolidBrush();
		SolidBrush(ResourceManager* pManager, const D2D1_COLOR_F& color);
		SolidBrush(ResourceManager* pManager, const float r, const float g, const float b, const float a = 1.0f);
		SolidBrush(ResourceManager* pManager, const float brightness, const float a = 1.0f);
		SolidBrush(const SolidBrush& other) = delete;
		SolidBrush& operator=(const SolidBrush& other) = delete;
		SolidBrush(SolidBrush&& other) noexcept;
		SolidBrush& operator=(SolidBrush&& other) noexcept;
		~SolidBrush();
		void Release() override;
		ID2D1SolidColorBrush* Get() const;
		void SetColor(const D2D1_COLOR_F& color);
		void SetColor(const float r, const float g, const float b, const float a = 1.0f);
		void SetColor(const float brightness, const float a = 1.0f);
		void SetColor(const SolidBrush& other);
	private:
		ID2D1SolidColorBrush* m_pBrush;
	};

	class BitmapBrush : private IBasicResource
	{
	public:
		BitmapBrush();
		BitmapBrush(ResourceManager* pManager, ID2D1Bitmap* pBitmap);
		BitmapBrush(ResourceManager* pManager, ID2D1Bitmap* pBitmap, float offsetX, float offsetY);
		BitmapBrush(ResourceManager* pManager, ID2D1Bitmap* pBitmap,
			const D2D1_BITMAP_BRUSH_PROPERTIES& bitmapBrushProperties,
			const D2D1_BRUSH_PROPERTIES& brushProperties = D2D1::BrushProperties());
		BitmapBrush(const BitmapBrush& other) = delete;
		BitmapBrush& operator=(const BitmapBrush& other) = delete;
		BitmapBrush(BitmapBrush&& other) noexcept;
		BitmapBrush& operator=(BitmapBrush&& other) noexcept;
		~BitmapBrush();
		void Release() override;
		void SetTranslation(float x, float y);
		ID2D1BitmapBrush* Get() const;
	private:
		ID2D1BitmapBrush* m_pBrush;
	};

	class GradientStops : private IBasicResource
	{
	public:
		GradientStops();
		GradientStops(ResourceManager* pManager, unsigned int expectedCount = 0);
		GradientStops(const GradientStops& other) = delete;
		GradientStops& operator=(const GradientStops& other) = delete;
		GradientStops(GradientStops&& other) noexcept;
		GradientStops& operator=(GradientStops&& other) noexcept;
		~GradientStops();
		void CopyFrom(const GradientStops& other);
		void AddStop(float pos, const D2D1_COLOR_F& color);
		void Recreate();
		ID2D1GradientStopCollection* Get();
		void Release() override;
		size_t StopCount() const;
	private:
		ID2D1GradientStopCollection* m_pStops;
		std::vector<D2D1_GRADIENT_STOP> m_vecStops;
	};

	class LinearBrush : private IBasicResource
	{
	public:
		LinearBrush();
		LinearBrush(ResourceManager* pManager, ID2D1GradientStopCollection* pStops,
			const D2D_POINT_2F& start, const D2D_POINT_2F& end);
		LinearBrush(ResourceManager* pManager, ID2D1GradientStopCollection* pStops,
			const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& gradientProperies,
			const D2D1_BRUSH_PROPERTIES& brushProperties = D2D1::BrushProperties());
		LinearBrush(const LinearBrush& other) = delete;
		LinearBrush& operator=(const LinearBrush& other) = delete;
		LinearBrush(LinearBrush&& other) noexcept;
		LinearBrush& operator=(LinearBrush&& other) noexcept;
		~LinearBrush();
		void Release() override;
		ID2D1LinearGradientBrush* Get();
		void SetPos(const D2D_POINT_2F& start, const D2D_POINT_2F& end);
	private:
		ID2D1LinearGradientBrush* m_pBrush;
	};

	class RadialBrush : private IBasicResource
	{
	public:
		RadialBrush();
		RadialBrush(ResourceManager* pManager, ID2D1GradientStopCollection* pStops,
			const D2D1_POINT_2F& center,
			const D2D1_POINT_2F& gradientOriginOffset,
			float radiusX, float radiusY
		);
		RadialBrush(ResourceManager* pManager, ID2D1GradientStopCollection* pStops,
			const D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES& gradientProperies,
			const D2D1_BRUSH_PROPERTIES& brushProperties = D2D1::BrushProperties());
		RadialBrush(const RadialBrush& other) = delete;
		RadialBrush& operator=(const RadialBrush& other) = delete;
		RadialBrush(RadialBrush&& other) noexcept;
		RadialBrush& operator=(RadialBrush&& other) noexcept;
		~RadialBrush();
		void Release() override;
		ID2D1RadialGradientBrush* Get();
		void SetCenter(const D2D_POINT_2F& center);
		D2D_POINT_2F GetCenter();
		void SetOffset(const D2D_POINT_2F& offset);
		D2D_POINT_2F GetOffset();
		void SetRadiusX(float x);
		void SetRadiusY(float y);
		void SetRadius(const D2D_POINT_2F& r);
		void SetRadius(float x, float y);
		void SetRadius(float r);
		float GetRadiusX();
		float GetRadiusY();
		D2D_POINT_2F GetRadius();
	private:
		ID2D1RadialGradientBrush* m_pBrush;
	};
}