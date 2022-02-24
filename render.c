
#include <SDL2/SDL.h>   // HEADER
#include <stdint.h>
#include "main.h"
#include "loop.h"


static float data [2][SIZE][SIZE][3];

// Prev & Now
#define P(X,Y) data[1&(frame-1)][Y&(SIZE-1)][X&(SIZE-1)]
#define N(X,Y) data[1&(frame+0)][Y&(SIZE-1)][X&(SIZE-1)]



inline uint8_t remap( float in ){
//    int out = 128*(1+in);  // [-1,+1] → [0,255]
    int out = 255*in;  // [0,1] → [0,255]
    if(out<0) return 0;
    if(out>255) return 255;
    return out;
}


void data_init(){   // HEADER
#pragma omp parallel for
    for( int y=0; y<SIZE; y++ ){
        for( int x=0; x<SIZE; x++ ){
            P(x,y)[0] = rand()*1.0/RAND_MAX;
            P(x,y)[1] = rand()*1.0/RAND_MAX;
            P(x,y)[2] = rand()*1.0/RAND_MAX;
        }
    }
}

// At each point in space and time, there are three concentrations u, v, and w 
// of chemicals, which are represented by the red, blue and green components 
// of an RGB color scheme. 
// Denoting by rho = u + v + w the total concentration, the system 
// of equations is given by 
// d_t u = D*Delta(u) + u*(1 - rho - a*v)
// d_t v = D*Delta(v) + v*(1 - rho - a*w)
// d_t w = D*Delta(w) + w*(1 - rho - a*u)
// where Delta denotes the Laplace operator,
// which performs a local average, and the parameter 
// a is equal here to 0.75.
// The terms proportional to a*v, a*w and a*u denote reaction terms,
// in which Red is beaten by Blue, Blue is beaten be Green, 
// and Green is beaten by Red.
// The situation is thus similar to the Rock-Paper-Scissors game 
// (see https://en.wikipedia.org/wiki/Rock_pa... ), and there exist
// simpler cellular automata with similar properties, see for instance 



void render( SDL_Event event ){  // HEADER

    uint8_t RGB8[SIZE][SIZE][3];

//#pragma omp parallel for
    for( int y=0; y<SIZE; y++ ){
        for( int x=0; x<SIZE; x++ ){

            float rho = 
                P(x,y)[0]+
                P(x,y)[1]+
                P(x,y)[2];

            // kernel quasi-gauss 
            // somma 16
            float avg[3] = {
                1*P(x-1,y-1)[0]+2*P(x+0,y-1)[0]+1*P(x+1,y-1)[0]+
                2*P(x-1,y+0)[0]+4*P(x+0,y+0)[0]+2*P(x+1,y+0)[0]+
                1*P(x-1,y+1)[0]+2*P(x+0,y+1)[0]+1*P(x+1,y+1)[0],

                1*P(x-1,y-1)[1]+2*P(x+0,y-1)[1]+1*P(x+1,y-1)[1]+
                2*P(x-1,y+0)[1]+4*P(x+0,y+0)[1]+2*P(x+1,y+0)[1]+
                1*P(x-1,y+1)[1]+2*P(x+0,y+1)[1]+1*P(x+1,y+1)[1],

                1*P(x-1,y-1)[2]+2*P(x+0,y-1)[2]+1*P(x+1,y-1)[2]+
                2*P(x-1,y+0)[2]+4*P(x+0,y+0)[2]+2*P(x+1,y+0)[2]+
                1*P(x-1,y+1)[2]+2*P(x+0,y+1)[2]+1*P(x+1,y+1)[2],
            };

            N(x,y)[0] = P(x,y)[0] + 0.01*(avg[0]*0.1 + P(x,y)[0]*(1-rho-P(x,y)[1]*0.75));
            N(x,y)[1] = P(x,y)[1] + 0.01*(avg[1]*0.1 + P(x,y)[1]*(1-rho-P(x,y)[2]*0.75));
            N(x,y)[2] = P(x,y)[2] + 0.01*(avg[2]*0.1 + P(x,y)[2]*(1-rho-P(x,y)[0]*0.75));

            RGB8[y][x][0] = remap(N(x,y)[0]);
            RGB8[y][x][1] = remap(N(x,y)[1]);
            RGB8[y][x][2] = remap(N(x,y)[2]);
        }
    }

    SDL_UpdateTexture( framebuffer , NULL, RGB8, SIZE*sizeof(RGB8[0][0]));
    SDL_RenderCopy( renderer, framebuffer , NULL , NULL );
}






