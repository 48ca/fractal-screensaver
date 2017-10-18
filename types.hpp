#ifndef TYPES_H
#define TYPES_H

struct info {
    int num_boxes;
    int num_boxes_down;
    int box_width;
    int box_height;
    int delay;
    int wait;
    int width;
    int height;
    double aspect;
} info;

template<typename T>
struct window {
    T start_x;
    T width_x;
    T start_y;
    int max_iters;
};

#endif // TYPES_H
