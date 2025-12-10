#pragma once
#include <raylib.h>
#include <string>
#include <jam/core/uuid.hpp>

namespace jam::components
{
    struct UIText
    {
        UUID id = 0;
        Font font = GetFontDefault();
        std::string text = "- TEXT HERE -";
        Vector2 origin = { 0, 0 };
        float fontSize = 20;
        float spacing = 1.0f;
        float rotation = 0;
        Color tint = WHITE;
        bool visible = true;

        Vector2 Measure() const
        {
            return MeasureTextEx(font, text.c_str(), fontSize, spacing);
        }

        void Render(Vector2 position, float angle) const
        {
            if (!visible) return;
            DrawTextPro(font, text.c_str(), position, origin, angle + rotation, fontSize, spacing, tint);
        }

    };
}