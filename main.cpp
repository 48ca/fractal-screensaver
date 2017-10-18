#include <SDL.h>
#include <stdlib.h>
#include <complex>
#include <cmath>
#include <array>

constexpr size_t SIMD_SIZE = 8;

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
    info.wait = 30; // in seconds
    info.iters = 70;
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

constexpr int color_map_length = 5;
uint8_t color_map[color_map_length][3] = {
    {0, 0, 0},
    {50, 0, 100},
    {255, 0, 255},
    {127, 0, 64},
    {0, 100, 255}
};
uint8_t loop_every = 10;

template<typename T, std::size_t N>
void genColors(uint8_t* colors, T power) {
    register int x;
    float aspect = (float)(info.height)/info.width;
    for(x = 0; x < info.width; x++) {
        float m_x = -.8 + 1.0 * x / info.width;
        poll();

        #pragma omp parallel for
        for(unsigned y = 0; y < info.height/N; y++) {
            std::array<std::complex<float>, N> m;
            for(unsigned i=0;i<N;++i) {
                m[i] = std::complex<float>(m_x, aspect * (1 + 1.0 * (y*N + i)/info.height));
            }
            std::array<unsigned, N> escaped{};
            std::array<int, N> iters{};
            std::array<std::complex<float>, N> zs;
            register int iter;
            for(iter=0;iter<info.iters;++iter) {
                register unsigned n_esc = N;
                for(unsigned k = 0; k < N; ++k) { n_esc -= escaped[k]; }
                if(!n_esc) break;

                #pragma omp simd
                for(unsigned k = 0; k < N; ++k) {
                    if(!escaped[k])
                    {
                        if(std::abs(zs[k]) > 2) {
                            escaped[k] = 1;
                            iters[k] = iter;
                        } else {
                            zs[k] = std::pow(zs[k], power) + m[k];
                        }
                    }
                }
            }
            for(unsigned i = 0; i < N; ++i) {
                float l = (float)iters[i]/loop_every;
                float p = l - std::floor(l);
                int f = (int)std::floor(l) % color_map_length;
                int c = (int)std::ceil(l) % color_map_length;
                register int k;
                for(k=0;k<3;++k)
                    colors[(y * N + i) * info.width * 3 + x * 3 + k] = (uint8_t)(color_map[f][k] * (1-p) + color_map[c][k] * p);
            }
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

SDL_Surface* scr;
uint8_t* colors;

void exit() {
    free(colors);
    SDL_FreeSurface(scr);
    SDL_Quit();
}

int main()
{
    std::atexit(exit);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_ShowCursor(0);

    genInfo();

    SDL_WM_SetCaption("SDL Test", NULL);

    scr = SDL_SetVideoMode(info.width, info.height, 32, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF);

    format = scr->format;
    surface = SDL_CreateRGBSurface(SDL_HWSURFACE, info.width, info.height, format->BitsPerPixel,
            format->Rmask, format->Gmask, format->Bmask, format->Amask);

    size_t colors_length = sizeof(uint8_t) * info.width * info.height * 3;
    colors = (uint8_t*)malloc(colors_length);

    float p;
    for(;;) {
        for(p = 2; p < 10; p+=1) {
            genColors<float, SIMD_SIZE>(colors, p);
            render(scr, colors);
            wait(info.wait);
            // render(screen, NULL);
        }
    }

    return 0;

}
