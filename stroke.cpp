#include "stroke.h"
#include <raymath.h>

using namespace std;

StrokeRecorder::StrokeRecorder() 
    : isCurrentlyDrawing(false) 
{
}

bool StrokeRecorder::Update(Vector2 mouseWorldPos, float currentZoom)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        isCurrentlyDrawing = true;
        float minSpacing = 2.5f / currentZoom;

        if (rawStroke.empty() || Vector2Distance(rawStroke.back(), mouseWorldPos) > minSpacing)
        {
            rawStroke.push_back(mouseWorldPos);
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && isCurrentlyDrawing)
    {
        isCurrentlyDrawing = false;
        if (rawStroke.size() > 10)
        {
            ProcessStroke(300); //Increase to get more strokes
            return true; 
        }
        rawStroke.clear();
    }
    return false;
}

void StrokeRecorder::ProcessStroke(int targetCount)
{
    rawStroke.push_back(rawStroke.front());
    vector<float> dists(rawStroke.size(), 0.0f);
    float totalLength = 0.0f;
    for (size_t i = 1; i < rawStroke.size(); ++i)
    {
        totalLength += Vector2Distance(rawStroke[i - 1], rawStroke[i]);
        dists[i] = totalLength;
    }
    processedPoints.clear();
    processedPoints.reserve(targetCount);
    float step = totalLength / (float)targetCount;
    size_t currIdx = 0;

    for (int i = 0; i < targetCount; ++i)
    {
        float targetDist = (float)i * step;
        while (currIdx < rawStroke.size() - 2 && dists[currIdx + 1] < targetDist)
        {
            currIdx++;
        }
        float segLen = dists[currIdx + 1] - dists[currIdx];
        float t = (segLen > 1e-4f) ? (targetDist - dists[currIdx]) / segLen : 0.0f;
        processedPoints.push_back(Vector2Lerp(rawStroke[currIdx], rawStroke[currIdx + 1], t));
    }

    Vector2 centroid = { 0.0f, 0.0f };
    for (const auto& p : processedPoints)
    {
        centroid.x += p.x;
        centroid.y += p.y;
    }
    centroid.x /= (float)processedPoints.size();
    centroid.y /= (float)processedPoints.size();

    for (auto& p : processedPoints)
    {
        p.x -= centroid.x;
        p.y -= centroid.y;
    }
}

void StrokeRecorder::Draw(float currentZoom) const
{
    if (rawStroke.size() >= 2)
    {
        DrawLineStrip(rawStroke.data(), (int)rawStroke.size(), YELLOW);
        DrawLineEx(rawStroke.back(), rawStroke.front(), 1.0f / currentZoom, Fade(YELLOW, 0.4f));
    }
}

void StrokeRecorder::Reset()
{
    rawStroke.clear();
    processedPoints.clear();
    isCurrentlyDrawing = false;
}