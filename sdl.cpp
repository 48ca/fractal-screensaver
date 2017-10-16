#include <SDL.h>

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WM_SetCaption("SDL Test", "SDL Test");

    SDL_Surface* screen;

    const SDL_VideoInfo* vidInfo = SDL_GetVideoInfo();   //<-- calls SDL_GetVideoInfo();

    const int width = vidInfo->current_w;
    const int height = vidInfo->current_h;

    screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_FULLSCREEN);

    SDL_Event event;

    SDL_Delay(1000);

    SDL_Quit();

    return 0;

}
