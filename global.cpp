
#include <SDL2/SDL.h>   // HEADER

#define COUNT(ARR) (sizeof(ARR)/sizeof(ARR[0]))     // HEADER

SDL_Texture  *framebuffer_tex;  // HEADER
SDL_Renderer *renderer; // HEADER

int frame;  // HEADER

// HEADERBEG

#define GRIDW (1<<9)
#define GRIDH (1<<8)
#define WINW  (GRIDW*2)
#define WINH  (GRIDH*2)

typedef struct {
    float G [GRIDH][GRIDW];
} GRID;


// HEADEREND


