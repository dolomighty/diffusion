
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>   // HEADER
#include <assert.h>
#include "loop.h"


#define SIZE (1<<7)     // HEADER
#define WID (SIZE*3)    // HEADER
#define HGT (SIZE*3)    // HEADER


SDL_Texture  *framebuffer;  // HEADER
SDL_Renderer *renderer;     // HEADER



int main( int argc, char *argv[] ){ 
        
    SDL_Window *win;
    assert( win = SDL_CreateWindow( argv[0], 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            WID, HGT, 0 ));

    assert( renderer = SDL_CreateRenderer( win, -1, 0 ));
    assert( framebuffer = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, SIZE, SIZE ));

    loop( renderer );
    
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( win );
    
    return 0;
}


