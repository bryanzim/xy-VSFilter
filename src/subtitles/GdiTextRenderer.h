#pragma once

#include <windows.h>

#include "TextRendererMode.h"

class GdiTextRenderer
{
public:
    GdiTextRenderer(HDC hdc, LPCWSTR text, int length, TextRendererMode mode);
    ~GdiTextRenderer();

    GdiTextRenderer(const GdiTextRenderer&) = delete;
    GdiTextRenderer& operator=(const GdiTextRenderer&) = delete;

    [[nodiscard]] bool GetExtent(SIZE* extent) const;
    [[nodiscard]] bool Draw(int x, int y) const;

private:
    [[nodiscard]] bool NeedsFallback() const;

    HDC m_hdc;
    LPCWSTR m_text;
    int m_length;
    void* m_analysis;
};
