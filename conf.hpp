#include "types.hpp"

#ifndef CONF_H
#define CONF_H

constexpr size_t SIMD_SIZE = 16;

void genInfo() {

    const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo();

    info.width = vidInfo->current_w;
    info.height = vidInfo->current_h;
    info.aspect = (double)(info.height)/info.width;
    info.num_boxes = 30; // number of boxes across
    info.delay = 15; // in ms
    info.wait = 30; // in seconds
    info.box_width = info.width/info.num_boxes;
    info.box_height = info.box_width;
    info.num_boxes_down = info.height/info.box_height;

}

constexpr int color_map_length = 5;
uint8_t color_map[color_map_length][3] = {
    {0, 0, 0},
    {50, 0, 100},
    {255, 0, 255},
    {127, 0, 64},
    {0, 100, 255}
};
uint8_t loop_every = 15;

struct window<float> float_windows[] = {
    // { x-low-start, x-width, y-low-start, max-iterations }
    {0, 0.26, 1.0405, 200},
    {-1.945, .005, -0.0025, 300},
    {-.8, 1.0, 1.0, 70}
};
struct window<double> double_windows[] = {
    {-0.7665, 0.025, 0.1202, 200}
};

int num_float_windows = sizeof(float_windows)/sizeof(struct window<float>);
int num_double_windows = sizeof(double_windows)/sizeof(struct window<double>);

#endif // CONF_H
