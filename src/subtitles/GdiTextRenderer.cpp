#include "stdafx.h"
#include "GdiTextRenderer.h"

#include <algorithm>
#include <cstdint>
#include <vector>

#include <usp10.h>

GdiTextRenderer::GdiTextRenderer(HDC hdc, LPCWSTR text, const int length, TextRendererMode mode)
    : m_hdc(hdc)
    , m_text(text)
    , m_length(length)
    , m_analysis(nullptr)
{
    mode = NormalizeTextRendererMode(mode);
    if ((m_length <= 0)
        || (mode == TEXT_RENDERER_LEGACY_GDI)
        || ((mode == TEXT_RENDERER_AUTO_FALLBACK) && !NeedsFallback()))
    {
        return;
    }

    SCRIPT_STRING_ANALYSIS analysis{nullptr};
    const int glyph_capacity{m_length + m_length / 2 + 16};
    const HRESULT hr{ScriptStringAnalyse(
        m_hdc,
        m_text,
        m_length,
        glyph_capacity,
        -1,
        SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
        0,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &analysis)};
    if (hr == S_OK)
    {
        m_analysis = analysis;
    }
    else if (analysis != nullptr)
    {
        ScriptStringFree(&analysis);
    }
}

GdiTextRenderer::~GdiTextRenderer()
{
    if (m_analysis != nullptr)
    {
        SCRIPT_STRING_ANALYSIS analysis{static_cast<SCRIPT_STRING_ANALYSIS>(m_analysis)};
        ScriptStringFree(&analysis);
        m_analysis = nullptr;
    }
}

[[nodiscard]] bool GdiTextRenderer::GetExtent(SIZE* extent) const
{
    if (m_analysis != nullptr)
    {
        const SIZE* size{ScriptString_pSize(static_cast<SCRIPT_STRING_ANALYSIS>(m_analysis))};
        if (size != nullptr)
        {
            *extent = *size;
            return true;
        }
    }

    return GetTextExtentPoint32W(m_hdc, m_text, m_length, extent) != FALSE;
}

[[nodiscard]] bool GdiTextRenderer::Draw(const int x, const int y) const
{
    if (m_analysis != nullptr)
    {
        return ScriptStringOut(
            static_cast<SCRIPT_STRING_ANALYSIS>(m_analysis),
            x,
            y,
            0,
            nullptr,
            0,
            0,
            FALSE) == S_OK;
    }

    return TextOutW(m_hdc, x, y, m_text, m_length) != FALSE;
}

[[nodiscard]] bool GdiTextRenderer::NeedsFallback() const
{
    std::vector<std::uint16_t> glyphs(static_cast<std::size_t>(m_length));
    if (GetGlyphIndicesW(
            m_hdc,
            m_text,
            m_length,
            glyphs.data(),
            GGI_MARK_NONEXISTING_GLYPHS) == GDI_ERROR)
    {
        return false;
    }

    constexpr std::uint16_t missing_glyph{0xFFFF};
    return std::find(glyphs.begin(), glyphs.end(), missing_glyph) != glyphs.end();
}
