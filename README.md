# TinyCircles

A 2D drawing machine built in C++ using Raylib. It takes freehand mouse sketches and reconstructs them using rotating epicycles calculated with the Discrete Fourier Transform (DFT).

---

## What It Does

TinyCircles lets you draw any closed or continuous shape on a canvas with your mouse. Once drawn, the application analyzes the stroke and breaks it down into a series of rotating vectors (circles). These vectors are chained together tip-to-tail, with each rotating at a specific frequency and radius, to retrace your original drawing.

---

## How It Works

1. **Path Sampling:** As you draw on the canvas, your mouse coordinates are recorded sequentially as a discrete time-series signal of 2D coordinates `(x, y)`.
2. **Discrete Fourier Transform (DFT):** The collected points are treated as complex numbers (`x + i*y`). The application computes the DFT to extract the frequency, amplitude (circle radius), and phase (starting angle) of every component.
3. **Sorting by Magnitude:** The resulting circles are sorted in descending order based on their radii so that the largest circles form the base of the chain, while smaller circles add finer details.
4. **Epicycle Animation:** The program updates the angle of each vector over time and adds them together tip-to-tail. The tip of the final vector traces out the reconstructed curve onto the screen.

---

## Controls

* **Left Click + Drag:** Draw your sketch on the screen
* **Space:** Start or pause the Fourier reconstruction animation
* **C:** Clear the canvas to start a new sketch
* **Escape:** Close the application

---

## Building from Source

### Prerequisites
* A C++ compiler supporting C++17 or later (GCC / MinGW / Clang)
* Raylib library installed

### Compilation
Compile all source files together using your preferred compiler:

```bash
g++ main.cpp dft.cpp moveables.cpp stroke.cpp -o TinyCircles.exe -lraylib -lopengl32 -lgdi32 -lwinmm
