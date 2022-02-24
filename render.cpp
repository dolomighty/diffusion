
#include <SDL2/SDL.h>
#include <stdint.h>
#include "gauss.h"
#include "global.h"
#include <time.h>



// 3 componenti per griglia
static GRID grid[3];

// grid addressing
#define P(X,Y,C) grid[C].G[Y][X]
#define G(X,Y,C) gau[C].G[Y][X]







void init(){   // HEADER

    srand(time(0));

//#pragma omp parallel for
//    for( int y=0; y<GRIDH; y++ ){
//        for( int x=0; x<GRIDW; x++ ){
//            P(x,y,0) = rand()*1.0/RAND_MAX;
//            P(x,y,1) = rand()*1.0/RAND_MAX;
//            P(x,y,2) = rand()*1.0/RAND_MAX;
//        }
//    }

    bzero( &grid, sizeof(grid));

#pragma omp parallel for
    for( int i=100; i>0; i-- ){
        int x = rand()&(GRIDW-1);
        int y = rand()&(GRIDH-1);
        int c = rand()%3;
        P(x,y,c) = rand()*1.0/RAND_MAX;
    }
}






inline uint8_t remap_u8( float in ){
//    int out = 128*(1+in);  // [-1,+1] → [0,255]
    int out = 255*in;  // [0,1] → [0,255]
    if(out<0) return 0;
    if(out>255) return 255;
    return out;
}


inline void colorize( 
    float *accum,
    float luma,
    const float r,
    const float g,
    const float b 
){
    if(luma<0)luma=0;
    if(luma>1)luma=1;
    // luma = [0,1]
    accum[0] += luma*r;
    accum[1] += luma*g;
    accum[2] += luma*b;
}




void render(){  // HEADER

    const float DT=1;

    uint8_t RGB8[GRIDH][GRIDW][3];

    GRID gau[3];
    gauss_blur( gau[0], grid[0]);
    gauss_blur( gau[1], grid[1]);
    gauss_blur( gau[2], grid[2]);


#pragma omp parallel for
    for( int y=0; y<GRIDH; y++ ){
        for( int x=0; x<GRIDW; x++ ){

            float rho =
                P(x,y,0)+
                P(x,y,1)+
                P(x,y,2)+
                0;

            float one_minus_rho = 1 - rho;

//            RGB8[y][x][0]=
//            RGB8[y][x][1]=
//            RGB8[y][x][2]=remap_u8(one_minus_rho*3);

            float P0 = P(x,y,0) + DT*(G(x,y,0)*0.1 + P(x,y,0)*(one_minus_rho-P(x,y,1)*0.9));
            float P1 = P(x,y,1) + DT*(G(x,y,1)*0.1 + P(x,y,1)*(one_minus_rho-P(x,y,2)*0.9));
            float P2 = P(x,y,2) + DT*(G(x,y,2)*0.1 + P(x,y,2)*(one_minus_rho-P(x,y,0)*0.9));

            P(x,y,0) = P0;
            P(x,y,1) = P1;
            P(x,y,2) = P2;

            float rgb[3]={0};

            colorize( rgb, P(x,y,0), 1.0, 0.2, 0.2 );
            colorize( rgb, P(x,y,1), 0.2, 0.2, 1.0 );
            colorize( rgb, P(x,y,2), 1.0, 1.0, 1.0 );

            RGB8[y][x][0]=remap_u8(rgb[0]);
            RGB8[y][x][1]=remap_u8(rgb[1]);
            RGB8[y][x][2]=remap_u8(rgb[2]);
        }
    }

    SDL_UpdateTexture( framebuffer_tex , NULL, RGB8, GRIDW*sizeof(RGB8[0][0]));
    SDL_RenderCopy( renderer, framebuffer_tex , NULL , NULL );
}




//void render(){  // HEADER
//
//    uint8_t RGB8[SIZE][SIZE][3];
//
//    GRID gau[3];
//    gauss_blur( gau[0], grid[1][0]);
//    gauss_blur( gau[1], grid[1][1]);
//    gauss_blur( gau[2], grid[1][2]);
//
//#pragma omp parallel for
//    for( int y=0; y<SIZE; y++ ){
//        for( int x=0; x<SIZE; x++ ){
//            if(frame&128){
//                RGB8[y][x][0] = remap_u8(G(x,y,0));
//                RGB8[y][x][1] = remap_u8(G(x,y,1));
//                RGB8[y][x][2] = remap_u8(G(x,y,2));
//            }else{
//                RGB8[y][x][0] = remap_u8(grid[1][0].G[y][x]);
//                RGB8[y][x][1] = remap_u8(grid[1][1].G[y][x]);
//                RGB8[y][x][2] = remap_u8(grid[1][2].G[y][x]);
//            }
//        }
//    }
//
//    SDL_UpdateTexture( framebuffer_tex , NULL, RGB8, GRIDW*sizeof(RGB8[0][0]));
//    SDL_RenderCopy( renderer, framebuffer_tex , NULL , NULL );
//}




/*


At each point in space and time, there are three concentrations u, v, and w 
of chemicals, which are represented by the red, blue and green components 
of an RGB color scheme. 
Denoting by rho = u + v + w the total concentration, the system 
of equations is given by 
d_t u = D*Delta(u) + u*(1 - rho - a*v)
d_t v = D*Delta(v) + v*(1 - rho - a*w)
d_t w = D*Delta(w) + w*(1 - rho - a*u)
where Delta denotes the Laplace operator,
which performs a local average, and the parameter 
a is equal here to 0.75.
The terms proportional to a*v, a*w and a*u denote reaction terms,
in which Red is beaten by Blue, Blue is beaten be Green, 
and Green is beaten by Red.
The situation is thus similar to the Rock-Paper-Scissors game 
(see https://en.wikipedia.org/wiki/Rock_pa... ), and there exist
simpler cellular automata with similar properties, see for instance 



*/

