

#include <SDL2/SDL.h>
#include <assert.h>
#include "loop.h"
#include "global.h"



int main( int argc, char *argv[] ){
        
    SDL_Window *win;
    assert( win = SDL_CreateWindow( argv[0], 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            WINW, WINH, 0 ));

    assert( renderer = SDL_CreateRenderer( win, -1, 0 ));
    assert( framebuffer_tex = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, GRIDW, GRIDH ));

    loop( renderer );
    
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( win );
    
    return 0;
}


