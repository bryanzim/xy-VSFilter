#pragma once

enum TextRendererMode : int
{
    TEXT_RENDERER_LEGACY_GDI = 0,
    TEXT_RENDERER_AUTO_FALLBACK,
    TEXT_RENDERER_UNISCRIBE,
    TEXT_RENDERER_MODE_COUNT
};

[[nodiscard]] inline TextRendererMode NormalizeTextRendererMode(const int value)
{
    if ((value < TEXT_RENDERER_LEGACY_GDI) || (value >= TEXT_RENDERER_MODE_COUNT))
    {
        return TEXT_RENDERER_LEGACY_GDI;
    }

    return static_cast<TextRendererMode>(value);
}
