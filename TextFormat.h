#pragma once
#include "ResourceManager.h"
#include <dwrite.h>

namespace Ice2D
{
	class TextFormat : private IBasicResource
	{
	public:
		TextFormat();
		TextFormat(ResourceManager* pManager, const wchar_t* fontName, const float fontSize,
			DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL);
		TextFormat(const TextFormat& other) = delete;
		TextFormat& operator=(const TextFormat& other) = delete;
		TextFormat(TextFormat&& other) noexcept;
		TextFormat& operator=(TextFormat&& other) noexcept;
		~TextFormat();
		void Release() override;
		IDWriteTextFormat* Get() const;
	private:
		IDWriteTextFormat* m_pFormat;
	};
}