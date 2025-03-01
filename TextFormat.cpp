#include "pch.h"

#include "TextFormat.h"
#include "HRException.h"
#include "SafeRelease.h"

namespace Ice2D
{
    TextFormat::TextFormat() : m_pFormat(nullptr)
    {
    }

    TextFormat::TextFormat(ResourceManager* pManager, const wchar_t* fontName, const float fontSize,
        DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch) :
        IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetWriteFactory()->CreateTextFormat(fontName, NULL, fontWeight, fontStyle, fontStretch,
            fontSize, L"", &m_pFormat);
        CheckHR(hr);

        OnLoad();
    }

    TextFormat::TextFormat(TextFormat&& other) noexcept : 
        IBasicResource(other), m_pFormat(other.m_pFormat)
    {
        other.m_pFormat = nullptr;
        OnLoad();
    }

    TextFormat& TextFormat::operator=(TextFormat&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pFormat = other.m_pFormat;
        other.m_pFormat = nullptr;

        OnLoad();
        return *this;
    }

    TextFormat::~TextFormat()
    {
        Release();
    }

    void TextFormat::Release()
    {
        SafeRelease(m_pFormat);
        OnUnload();
    }

    IDWriteTextFormat* TextFormat::Get() const
    {
        if (!m_pFormat) throw std::runtime_error("Text format is null.");
        return m_pFormat;
    }
}
