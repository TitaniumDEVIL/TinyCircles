#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include "dft.h"
#include "moveables.h"
#include "stroke.h"

using namespace std;

enum MachineState { STATE_DRAWING, STATE_SIMULATING };

void DrawInfiniteGrid(Camera2D camera, int cellSize);

int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "Fourier Epicycle Machine");
    SetTargetFPS(120);

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ screenWidth * 0.5f, screenHeight * 0.5f };

    MachineState state = STATE_DRAWING;
    StrokeRecorder recorder;

    vector<Epicycle> epicycles;
    vector<Vector2> trail;

    float time = 0.0f;
    float speedMultiplier = 1.0f;
    bool showRings = true;
    bool isPaused = false;

    const float baseDt = 0.0035f;
    const int subSteps = 5;
    const float ringThickness = 1.5f;

    const Color DarkGrey   = { 20, 20, 20, 255 };
    const Color RingColor  = { 80, 80, 80, 200 };
    const Color TrailColor = { 0, 220, 255, 255 };

    while (!WindowShouldClose())
    {
        Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f)
        {
            camera.offset = GetMousePosition();
            camera.target = mouseWorld;
            camera.zoom = std::clamp(camera.zoom + wheel * 0.125f, 0.1f, 15.0f);
        }

        if (state == STATE_DRAWING)
        {
            if (recorder.Update(mouseWorld, camera.zoom))
            {
                epicycles = ComputeDFT(recorder.GetProcessedPoints());
                time = 0.0f;
                trail.clear();
                state = STATE_SIMULATING;
            }
        }
        else if (state == STATE_SIMULATING)
        {
            if (IsKeyPressed(KEY_R))
            {
                recorder.Reset();
                epicycles.clear();
                trail.clear();
                state = STATE_DRAWING;
            }
            if (IsKeyDown(KEY_UP))       speedMultiplier = min(speedMultiplier + 0.02f, 4.0f);
            if (IsKeyDown(KEY_DOWN))     speedMultiplier = max(speedMultiplier - 0.02f, 0.1f);
            if (IsKeyPressed(KEY_SPACE))  isPaused = !isPaused;
            if (IsKeyPressed(KEY_H))      showRings = !showRings;

            if (!isPaused && !epicycles.empty())
            {
                float stepDt = baseDt * speedMultiplier;

                for (int step = 0; step < subSteps; ++step)
                {
                    time += stepDt;

                    if (time >= 2.0f * PI)
                    {
                        time = 0.0f;
                        trail.clear();
                        break;
                    }
                    Vector2 stepTip = { 0.0f, 0.0f };
                    for (const auto& ep : epicycles)
                    {
                        float angle = (ep.freq * time) + ep.phase;
                        float effRadius = fmaxf(0.0f, ep.radius - ringThickness);
                        stepTip.x += effRadius * cosf(angle);
                        stepTip.y += effRadius * sinf(angle);
                    }
                    trail.push_back(stepTip);
                }
            }
        }
        BeginDrawing();
            ClearBackground(DarkGrey);
            BeginMode2D(camera);
                DrawInfiniteGrid(camera, 50);

                if (state == STATE_DRAWING)
                {
                    recorder.Draw(camera.zoom);
                }
                else if (state == STATE_SIMULATING)
                {
                    Vector2 currentCenter = { 0.0f, 0.0f };
                    for (const auto& ep : epicycles)
                    {
                        float currentAngle = (ep.freq * time) + ep.phase;
                        if (showRings)
                        {
                            currentCenter = DrawArrowRing(currentCenter, ep.radius, ringThickness, currentAngle, RingColor);
                        }
                        else
                        {
                            float effRadius = fmaxf(0.0f, ep.radius - ringThickness);
                            currentCenter.x += effRadius * cosf(currentAngle);
                            currentCenter.y += effRadius * sinf(currentAngle);
                        }
                    }
                    Vector2 penTip = currentCenter;
                    if (trail.size() >= 2)
                    {
                        DrawLineStrip(trail.data(), (int)trail.size(), TrailColor);
                    }
                    DrawCircleV(penTip, 3.5f / camera.zoom, WHITE);
                }
            EndMode2D();
            if (state == STATE_DRAWING)
            {
                DrawText("DRAWING MODE: Click & drag Left Mouse Button to draw", 20, 20, 20, YELLOW);
                DrawText("- Release mouse to calculate DFT and run", 20, 50, 16, LIGHTGRAY);
                DrawText("- Right-Click Drag: Pan | Scroll: Zoom", 20, 75, 16, GRAY);
            }
            else
            {
                DrawText("SIMULATION MODE: Press [R] to reset", 20, 20, 20, SKYBLUE);
                DrawText("[SPACE] Pause | [UP/DOWN] Speed | [H] Toggle Rings", 20, 50, 16, LIGHTGRAY);
                DrawText(TextFormat("Harmonics: %d epicycles", (int)epicycles.size()), 20, 75, 16, GRAY);
            }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void DrawInfiniteGrid(Camera2D camera, int cellSize)
{
    Color gridColor = Fade(LIGHTGRAY, 0.08f);

    Vector2 topLeft = GetScreenToWorld2D((Vector2){ 0, 0 }, camera);
    Vector2 bottomRight = GetScreenToWorld2D((Vector2){ (float)GetScreenWidth(), (float)GetScreenHeight() }, camera);

    int startX = ((int)topLeft.x / cellSize) * cellSize - cellSize;
    int endX   = ((int)bottomRight.x / cellSize) * cellSize + cellSize;
    int startY = ((int)topLeft.y / cellSize) * cellSize - cellSize;
    int endY   = ((int)bottomRight.y / cellSize) * cellSize + cellSize;

    for (int x = startX; x <= endX; x += cellSize) DrawLine(x, startY, x, endY, gridColor);
    for (int y = startY; y <= endY; y += cellSize) DrawLine(startX, y, endX, y, gridColor);

    DrawLine(-20, 0, 20, 0, Fade(RED, 0.4f));
    DrawLine(0, -20, 0, 20, Fade(GREEN, 0.4f));
}