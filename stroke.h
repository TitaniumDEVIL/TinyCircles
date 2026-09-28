#pragma once
#include <vector>
#include "raylib.h"

using namespace std;

class StrokeRecorder 
{
public:
    StrokeRecorder();
    bool Update(Vector2 mouseWorldPos, float currentZoom);
    void Draw(float currentZoom) const;
    void Reset();
    const vector<Vector2>& GetProcessedPoints() const { return processedPoints; }

private:
    vector<Vector2> rawStroke;
    vector<Vector2> processedPoints;
    bool isCurrentlyDrawing;

    void ProcessStroke(int targetCount = 300);
};