#include <vector>
#include "raylib.h"
#include "dft.h"
#include "raymath.h"
#include <cmath>
#include <algorithm>
using namespace std;

vector<Epicycle> ComputeDFT(const vector<Vector2> &points)
{
    vector<Epicycle> value;

    if (points.empty()) return value;
    
    int size = points.size();

    for (int k = -size/2; k < size/2; k++) {
        float x = 0.0f;
        float y = 0.0f;
        for (int i = 0; i < size; i++) {
            float theta = (2 * PI * (float)k * (float)i) / (float)size;
            x += points[i].x * cosf(theta) + points[i].y * sinf(theta);
            y += points[i].y * cosf(theta) - points[i].x * sinf(theta);
        }
        x = x /(float)size;
        y = y /(float)size;
        value.push_back({(float)k, sqrtf(x * x + y * y), atan2f(y, x)});
    }
    sort(value.begin(), value.end(), [](const Epicycle &a, const Epicycle &b) {
        return a.radius > b.radius;
    });

    //Increase this value if you want more circles, stroke is hardcoded to 300 points if you want to above that increase that too

    if (value.size() > 200) {
        value.resize(200);
    }
    return value;
}