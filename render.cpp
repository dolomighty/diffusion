

#include <SDL2/SDL.h>
#include <stdint.h>
#include "global.h"
#include <time.h>




static GRID con[3];     // 3 "reagenti"

// C sta per concentrazione dei reagenti
#define C(X,Y,C) con[C].G[Y][X]







void init(){   // HEADER

    bzero( &con, sizeof(con));

    srand(time(0));

//#pragma omp parallel for
//    for( int y=0; y<GRIDH; y++ ){
//        for( int x=0; x<GRIDW; x++ ){
//            C(x,y,0) = rand()*1.0/RAND_MAX;
//            C(x,y,1) = rand()*1.0/RAND_MAX;
//            C(x,y,2) = rand()*1.0/RAND_MAX;
//        }
//    }


#pragma omp parallel for
    for( int i=100; i>0; i-- ){
        int x = rand()&(GRIDW-1);
        int y = rand()&(GRIDH-1);
        int c = rand()%3;
        C(x,y,c) = rand()*1.0/RAND_MAX;
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

    // op laplaciano 2D
    // https://en.wikipedia.org/wiki/Discrete_Laplace_operator
    // "Implementation via operator discretization"

    GRID lapla[3];


#pragma omp parallel for
    for( int y=0; y<GRIDH; y++ ){
        for( int x=0; x<GRIDW; x++ ){

#define W(X,Y,C) con[C].G[(Y)&(GRIDH-1)][(X)&(GRIDW-1)]

            lapla[0].G[y][x]= W(x-1,y,0)+W(x+1,y,0)+W(x,y-1,0)+W(x,y+1,0)-4*W(x,y,0);
            lapla[1].G[y][x]= W(x-1,y,1)+W(x+1,y,1)+W(x,y-1,1)+W(x,y+1,1)-4*W(x,y,1);
            lapla[2].G[y][x]= W(x-1,y,2)+W(x+1,y,2)+W(x,y-1,2)+W(x,y+1,2)-4*W(x,y,2);
        }
    }




#pragma omp parallel for
    for( int y=0; y<GRIDH; y++ ){
        for( int x=0; x<GRIDW; x++ ){

            float rho =
                C(x,y,0)+
                C(x,y,1)+
                C(x,y,2)+
                0;

            float one_minus_rho = 1 - rho;

#define L(X,Y,C) lapla[C].G[Y][X]

            float N0 = C(x,y,0) + DT*(L(x,y,0)*0.1 + C(x,y,0)*(one_minus_rho-C(x,y,1)*0.5));
            float N1 = C(x,y,1) + DT*(L(x,y,1)*0.1 + C(x,y,1)*(one_minus_rho-C(x,y,2)*0.5));
            float N2 = C(x,y,2) + DT*(L(x,y,2)*0.1 + C(x,y,2)*(one_minus_rho-C(x,y,0)*0.5));

            C(x,y,0) = N0;
            C(x,y,1) = N1;
            C(x,y,2) = N2;

            float rgb[3]={0};

//            // w
//            colorize( rgb, C(x,y,0), 1, 1, 1 );
//            colorize( rgb, C(x,y,1), 1, 1, 1 );
//            colorize( rgb, C(x,y,2), 0, 0, 0 );

            // r b w
            colorize( rgb, C(x,y,0), 1.0, 1.0, 1.0 );
            colorize( rgb, C(x,y,1), 0.2, 0.2, 1.0 );
            colorize( rgb, C(x,y,2), 1.0, 0.2, 0.2 );

//            rgb[0]=one_minus_rho*3;
//            rgb[1]=one_minus_rho*3;
//            rgb[2]=one_minus_rho*3;

            RGB8[y][x][0]=remap_u8(rgb[0]);
            RGB8[y][x][1]=remap_u8(rgb[1]);
            RGB8[y][x][2]=remap_u8(rgb[2]);
        }
    }

    SDL_UpdateTexture( framebuffer_tex , NULL, RGB8, GRIDW*sizeof(RGB8[0][0]));
    SDL_RenderCopy( renderer, framebuffer_tex , NULL , NULL );
}





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


