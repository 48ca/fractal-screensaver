# Fractal Screensaver
I wrote this because I wanted something cool to run on my laptop while it sitting on my desk doing nothing.
I originally used C++'s complex variables to render a lot of fractals, but for speed and power concerns, I switched to just render the mandelbrot set.
The mandelbrot set is rendered using SIMD/SSE2 instructions (on all cores) for both speed and power efficiency.

Using SIMD with std::array takes heavy inspiration from Bisqwit.

# Configuration
You can edit the window areas and colors by changing some variables in `conf.hpp`.
