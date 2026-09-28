#include "moveables.h"
#include <cmath>
Vector2 DrawArrowRing(Vector2 center, float radius, float thickness, float phase, Color color)
{
    float innerRadius = fmax(0.0f, radius - thickness);
    DrawRing(center, innerRadius, radius, 0.0f, 360.0f, 720, color);
    float x = (radius - thickness) * cosf(phase);
    float y = (radius - thickness) * sinf(phase);
    Vector2 endPos = {center.x + x, center.y + y};
    DrawLineEx(center, endPos, thickness, color);
    DrawCircleV(endPos, 4.0f, color);
    return endPos;
}
