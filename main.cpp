#include <SDL.h>
#include <stdlib.h>
#include <complex>
#include <cmath>

struct info {
    int num_boxes;
    int num_boxes_down;
    int box_width;
    int box_height;
    int delay;
    int wait;
    int iters;
    int width;
    int height;
} info;

void genInfo() {

    const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo();

    info.width = vidInfo->current_w;
    info.height = vidInfo->current_h;
    info.num_boxes = 30; // number of boxes across
    info.delay = 15; // in ms
    info.wait = 5; // in seconds
    info.iters = 100;
    info.box_width = info.width/info.num_boxes;
    info.box_height = info.box_width;
    info.num_boxes_down = info.height/info.box_height;

}

SDL_Event event;
void poll() {
    SDL_PollEvent(&event);
    switch(event.type) {
        case SDL_QUIT:
        case SDL_KEYDOWN:
            exit(0);
    }
}

SDL_PixelFormat* format;
void setPixel(SDL_Surface *screen, int x, int y, uint8_t* colors)
{
    int width = screen->pitch;
    uint32_t *pixels = (uint32_t*) screen->pixels + y * (width / 4) + x;

    if(colors)
        *pixels = SDL_MapRGB(format, colors[0], colors[1], colors[2] );
    else
        *pixels = SDL_MapRGB(format, 0, 0, 0);
}

SDL_Surface* surface;
void render(SDL_Surface* screen, uint8_t* colors) {
    register int x, y;
    register int box_x, box_y;
    register int i;
    for (i = 0; i < info.num_boxes + info.num_boxes_down + 1; i++)
    {
        for (box_x = i, box_y = 0; box_x >= 0; box_x--, box_y++) {
            register int initial_x = box_x * info.box_width;
            register int initial_y = box_y * info.box_height;
            for (y = initial_y; y < info.height && y - initial_y < info.box_height; y++) {
                for (x = initial_x; x < info.width && x - initial_x < info.box_width; x++) {
                    setPixel(surface, x, y, colors ? colors + (sizeof(uint8_t) * (y*info.width*3 + x*3)) : NULL);
                }
            }
        }
        SDL_BlitSurface(surface, NULL, screen, NULL);
        SDL_Flip(screen);
        poll();
        SDL_Delay(info.delay);
    }
}

template <typename T>
int mandelIterations(std::complex<float> m, int max, T power) {
    std::complex<float> z = 0;
    int i;
    for(i = 0 ; i < max ; ++i) {
        if(std::abs(z) > 2) break;
        // Boat
        // z = std::pow(std::complex<float>(std::abs(std::real(z)), std::abs(std::imag(z))), power) + m;
        z = std::pow(z, power) + m;
    }
    return max-i;
}

constexpr int color_map_length = 5;
uint8_t color_map[color_map_length][3] = {
    {0, 0, 0},
    {0, 125, 0},
    {255, 0, 255},
    {127, 0, 64},
    {0, 100, 255}
};
uint8_t loop_every = 10;

template<typename T>
void genColors(uint8_t* colors, T power) {
    register int x;
    register int y;
    for(x = 0; x < info.width; x++) {
        poll();
        #pragma omp parallel for
        for(y = 0; y < info.height; y++) {
            float m_x = -.8 + 1.0 * x / info.width;
            float aspect = (float)(info.height)/info.width;
            float m_y = aspect * (1 + 1.0 * y/info.height);

            std::complex<float> m(m_x, m_y);
            int i = mandelIterations<T>(m, info.iters, power);
            float l = (float)i/loop_every;
            float p = l - std::floor(l);
            int f = (int)std::floor(l) % color_map_length;
            int c = (int)std::ceil(l) % color_map_length;
            register int k;
            #pragma omp simd
            for(k=0;k<3;++k)
                colors[y * info.width * 3 + x * 3 + k] = (uint8_t)(color_map[f][k] * (1-p) + color_map[c][k] * p);
        }
    }
}

constexpr int resolution = 10;
void wait(int sec) {
    register int i;
    register int j;
    for(i = 0; i < sec; ++i) {
        for(j = 0; j < resolution; ++j) {
            poll();
            SDL_Delay(1000/resolution);
        }
    }
}

void exit() {
    SDL_Quit();
}

int main()
{
    std::atexit(exit);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_ShowCursor(0);

    genInfo();

    SDL_WM_SetCaption("SDL Test", NULL);

    SDL_Surface* screen = SDL_SetVideoMode(info.width, info.height, 32, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF);

    format = screen->format;
    surface = SDL_CreateRGBSurface(SDL_HWSURFACE, info.width, info.height, format->BitsPerPixel,
            format->Rmask, format->Gmask, format->Bmask, format->Amask);

    size_t colors_length = sizeof(uint8_t) * info.width * info.height * 3;
    uint8_t* colors = (uint8_t*)malloc(colors_length);

    float p;
    for(;;) {
        for(p = 2; p < 10; p+=1) {
            genColors<float>(colors, p);
            render(screen, colors);
            wait(info.wait);
            // render(screen, NULL);
        }
    }

    SDL_FreeSurface(screen);

    return 0;

}
