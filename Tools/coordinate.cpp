#include "coordinate.h"

Vector2 coordinate::origin(float screenSizeX, float screenSizeY)
{
    float xCoordinate, yCoordinate;
    xCoordinate = screenSizeX / 2.0f;
    yCoordinate = screenSizeY / 2.0f;
    
    return {
    xCoordinate, yCoordinate
    };
}

Vector2 coordinate::middle_edge(int screenSizeX, int screenSizeY, enum positions positions)
{
    switch (positions)
    {
        case positions::left:
            return { 0, screenSizeY / 2.0f };
            break;
        
        case positions::right:
            return {(float)screenSizeX, screenSizeY / 2.0f};
            break;
        
        case positions::top:
            return {screenSizeX / 2.0f, 0};
            break;
        
        case positions::bottom:
            return {screenSizeX / 2.0f, (float)screenSizeY};
            break;
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
    Vector2 size = MeasureTextEx(font, text, fontSize, 1);
    DrawTextEx(font, text, { pos.x - size.x / 2, pos.y }, fontSize, 1, color);
}
