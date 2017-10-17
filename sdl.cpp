#include <SDL/SDL.h>
#include  <stdlib.h>
#include <complex>

struct info {
    int num_boxes;
    int num_boxes_down;
    int box_width;
    int box_height;
    int width;
    int height;
} info;

void setPixel(SDL_Surface *screen, int x, int y, Uint8* colors)
{
    int width = screen->pitch;
    Uint32 *pixels = (Uint32*) screen->pixels + y * (width / 4) + x;
    SDL_PixelFormat *format = screen->format;

    *pixels = SDL_MapRGB(format, colors[0], colors[1], colors[2] );
}

void render(SDL_Surface* screen, Uint8* colors) {
    SDL_PixelFormat* format = screen->format;
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE, info.width, info.height, format->BitsPerPixel,
            format->Rmask, format->Gmask, format->Bmask, format->Amask);

    register int x, y;
    register int box_x, box_y;
    register int i;
    for (i = 0; i < info.num_boxes + info.num_boxes_down; i++)
    {
        for (box_x = i, box_y = 0; box_x >= 0; box_x--, box_y++) {
            register int initial_x = box_x * info.box_width;
            register int initial_y = box_y * info.box_height;
            for (y = initial_y; y < info.height && y - initial_y < info.box_height; y++) {
                for (x = initial_x; x < info.width && x - initial_x < info.box_width; x++) {
                    setPixel(surface, x, y, colors + (sizeof(Uint8) * (y*info.width*3 + x*3)));
                }
            }
        }
        SDL_BlitSurface(surface, NULL, screen, NULL);
        SDL_Flip(screen);
        SDL_Delay(15);
    }
}

void genInfo() {

    const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo();   //<-- calls SDL_GetVideoInfo();

    info.width = vidInfo->current_w;
    info.height = vidInfo->current_h;
    info.num_boxes = 50; // number of boxes across
    info.box_width = info.width/info.num_boxes;
    info.box_height = info.box_width;
    info.num_boxes_down = info.height/info.box_height;

}

int mandelIterations(std::complex<float> m, int max, int power) {
    std::complex<float> z = 0;
    int i;
    for(i = 0 ; i < max ; ++i) {
        if(std::abs(z) > 2) break;
        z = std::pow(z, power) + m;
    }
    return max-i;
}

int max = 200;
void genColors(Uint8* colors, int power) {
    register int x;
    register int y;
    for(x = 0; x < info.width; x++) {
        for(y = 0; y < info.height; y++) {
            float m_x = -2 + 4.0 * x / info.width;
            float aspect = (float)(info.height)/info.width;
            float m_y = aspect * (-2 + 4.0 * y/info.height);

            std::complex<float> m(m_x, m_y);
            int i = mandelIterations(m, max, power);
            float f = (float)(i)/max;

            colors[y * info.width * 3 + x * 3] = (Uint8)(255.0 * f);
            colors[y * info.width * 3 + x * 3 + 1] = (Uint8)(255.0 * f);
            colors[y * info.width * 3 + x * 3 + 2] = (Uint8)(255.0 * f);
        }
    }
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_ShowCursor(0);

    genInfo();

    SDL_WM_SetCaption("SDL Test", NULL);

    SDL_Surface* screen = SDL_SetVideoMode(info.width, info.height, 32, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF);

    Uint8* colors = (Uint8*)malloc(sizeof(Uint8) * info.width * info.height * 3);

    int p;
    for(p = 2; p < 10; p++) {
        genColors(colors, p);
        render(screen, colors);
    }

    /* While the program is running
    while (!quit)
    {
        while(SDL_PollEvent(&event))
        {
        }

    }
    */

    SDL_FreeSurface(screen);

    SDL_Quit();

    return 0;

}
