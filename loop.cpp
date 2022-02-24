

#include <SDL2/SDL.h>   // HEADER
#include "render.h"
#include "global.h"
#include <stdint.h>
#include <time.h>


void loop( SDL_Renderer *renderer ){    // HEADER

    frame = 0;
    init();

    while(1){
        
        SDL_Event event;
        while( SDL_PollEvent(&event)){
            switch( event.type ){
                case SDL_KEYDOWN:
//                    fprintf( stderr, "SDL_KEYDOWN %s\n", SDL_GetKeyName(event.key.keysym.sym));
//                    switch( event.key.keysym.sym ){
//                        case SDLK_1 :
//                            fprintf( stderr, "1\n" );
//                        break;
//                    }
                    break;
                case SDL_KEYUP:
//                    fprintf( stderr, "SDL_KEYUP %s\n", SDL_GetKeyName(event.key.keysym.sym));
                    break;
                case SDL_MOUSEMOTION:
//                    fprintf( stderr, "SDL_MOUSEMOTION %d,%d abs %d,%d\n"
//                        ,event.motion.xrel
//                        ,event.motion.yrel
//                        ,event.motion.x
//                        ,event.motion.y
//                    );
                    break;
                case SDL_MOUSEBUTTONDOWN:
//                    fprintf( stderr, "SDL_MOUSEBUTTONDOWN %d @ %d,%d\n",
//                        event.button.button, 
//                        event.button.x, 
//                        event.button.y
//                    );
                    break;
                case SDL_MOUSEBUTTONUP:
//                    fprintf( stderr, "SDL_MOUSEBUTTONUP %d @ %d,%d\n",
//                        event.button.button, 
//                        event.button.x, 
//                        event.button.y
//                    );
                    break;
                case SDL_QUIT:
//                    fprintf( stderr, "SDL_QUIT\n" );
                    return;
            }
        }

        struct timespec start, end;
        clock_gettime(CLOCK_REALTIME, &start);

        render();

        clock_gettime(CLOCK_REALTIME, &end);
        float lap = (end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)*1E-9;
        fprintf(stderr,"lap %.4f secs\n",lap);

        frame++;

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }
}    


