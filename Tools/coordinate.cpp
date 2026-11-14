#include "Coordinate.h"

Vector2 Coordinate::origin(float screenSizeX, float screenSizeY)
{
    return { screenSizeX * 0.5f, screenSizeY * 0.5f };
}

Vector2 Coordinate::middle_edge(float screenSizeX, float screenSizeY, enum positions pos)
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

Vector2 Coordinate::worldToScreen(Vector2 world, Vector2 origin, float pixelsPerUnit)
{
    return {
        origin.x + world.x * pixelsPerUnit,
        origin.y - world.y * pixelsPerUnit
    };
}

Vector2 Coordinate::screenToWorld(Vector2 screen, Vector2 origin, float pixelsPerUnit)
{
    return {
        (screen.x - origin.x) / pixelsPerUnit,
        (origin.y - screen.y) / pixelsPerUnit
    };
}

void Coordinate::DrawTickLabel(Font font, int value, Vector2 pos, float fontSize, Color color)
{
    const char* text = TextFormat("%d", value);
    Vector2 size = MeasureTextEx(font, text, fontSize, 1.0f);
    DrawTextEx(font, text, { pos.x - size.x * 0.5f, pos.y }, fontSize, 1.0f, color);
}
