#include "Arrow.h"

void Arrow::DrawArrow(Vector2 start, Vector2 end, float thickness, float headSize, Color color)
{
    Vector2 delta = Vector2Subtract(end, start);
    if (Vector2Length(delta) < 1.0f) return;   // avoid zero-length direction

    Vector2 dir = Vector2Normalize(delta);

    // Shaft
    DrawLineEx(start, end, thickness, color);

    // Head: rotate the direction ±150° so the two lines point backwards from the tip
    Vector2 left  = Vector2Add(end, Vector2Scale(Vector2Rotate(dir,  150 * DEG2RAD), headSize));
    Vector2 right = Vector2Add(end, Vector2Scale(Vector2Rotate(dir, -150 * DEG2RAD), headSize));

    DrawLineEx(end, left,  thickness, color);
    DrawLineEx(end, right, thickness, color);
}