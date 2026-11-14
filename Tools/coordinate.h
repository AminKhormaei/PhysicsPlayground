#pragma once
#include "raylib.h"

class coordinate
{
    public:
        enum class positions { left, top, right, bottom };
        coordinate() = default; // default constructor
    
        Vector2 origin(float, float);   // Finding the center of the screen 
        Vector2 middle_edge(int, int, positions); // Coordinate of the left, top, right, bottom
        Vector2 worldToScreen(Vector2, Vector2, float);
        Vector2 screenToWorld(Vector2, Vector2, float);
        void DrawTickLabel(Font, int, Vector2, float, Color);
};
