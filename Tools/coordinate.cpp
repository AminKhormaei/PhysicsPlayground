#include "coordinate.h"

Vector2 coordinate::origin(float screenSizeX, float screenSizeY)
{
    return { screenSizeX * 0.5f, screenSizeY * 0.5f };
}

Vector2 coordinate::middle_edge(float screenSizeX, float screenSizeY, enum positions pos)
{
    switch (pos)
    {
        case positions::left:
            return { 0.0f, screenSizeY * 0.5f };
        case positions::right:
            return { screenSizeX, screenSizeY * 0.5f };
        case positions::top:
            return { screenSizeX * 0.5f, 0.0f };
        case positions::bottom:
            return { screenSizeX * 0.5f, screenSizeY };
        default:
            return { 0.0f, 0.0f };
    }
}

Vector2 coordinate::worldToScreen(Vector2 world, Vector2 origin, float pixelsPerUnit)
{
    return {
        origin.x + world.x * pixelsPerUnit,
        origin.y - world.y * pixelsPerUnit
    };
}

Vector2 coordinate::screenToWorld(Vector2 screen, Vector2 origin, float pixelsPerUnit)
{
    return {
        (screen.x - origin.x) / pixelsPerUnit,
        (origin.y - screen.y) / pixelsPerUnit
    };
}

void coordinate::DrawTickLabel(Font font, int value, Vector2 pos, float fontSize, Color color)
{
    const char* text = TextFormat("%d", value);
    Vector2 size = MeasureTextEx(font, text, fontSize, 1.0f);
    DrawTextEx(font, text, { pos.x - size.x * 0.5f, pos.y }, fontSize, 1.0f, color);
}
