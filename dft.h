#pragma once

#include <raylib.h>
#include <complex>
#include <vector>
#include <cstdint>
using namespace std;

using Complex = complex<float>;

struct Epicycle {
    float freq = 0.0f;
    float radius = 0.0f;
    float phase = 0.0f; 
};

vector<Epicycle> ComputeDFT(const vector<Vector2>& points);