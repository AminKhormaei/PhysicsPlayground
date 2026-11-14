#pragma once
#include "raylib.h"

class Coordinate
{
public:
    enum class positions { left, top, right, bottom };
    Coordinate() = default;

    static Vector2 origin(float screenSizeX, float screenSizeY);
    static Vector2 middle_edge(float screenSizeX, float screenSizeY, positions pos);
    static Vector2 worldToScreen(Vector2 world, Vector2 origin, float pixelsPerUnit);
    static Vector2 screenToWorld(Vector2 screen, Vector2 origin, float pixelsPerUnit);
    static void DrawTickLabel(Font font, int value, Vector2 pos, float fontSize, Color color);
};

