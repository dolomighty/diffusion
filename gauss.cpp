
#include "global.h"     // HEADER
#include <math.h>
#include <numeric>


#define COUNT(ARR) (sizeof(ARR)/sizeof(ARR[0]))
#define WRAP(X,Y) G[(Y)&(GRIDH-1)][(X)&(GRIDW-1)]



constexpr float gauss( float x, float dirac ){
    // dirac 0 → y=1
    // dirac ∞ → dirac delta
    return exp(-x*x*dirac);
}


void gauss_blur( GRID &out, const GRID &in ){   // HEADER
    // gaussian blur per parti

    // il kernel è simmetrico
    // quindi servon solo meta dei valori
    // math.exp(-9*9*0.05)= 0.017422374639493515
    // math.exp(-9*9*0.1)= 0.0003035391380788668
    // math.exp(-5*5*0.1)= 0.0820849986238988
    constexpr float kernel[]={
        gauss( 0, 0.2 ),
        gauss( 1, 0.2 ),
        gauss( 2, 0.2 ),
        gauss( 3, 0.2 ),
        gauss( 4, 0.2 ),
        gauss( 5, 0.2 ),
//        gauss( 6, 0.1 ),
//        gauss( 7, 0.1 ),
//        gauss( 8, 0.1 ),
//        gauss( 9, 0.1 ),
    };

    constexpr float one_over_sum = 1/pow(
        kernel[0]+2*(
            kernel[1]+
            kernel[2]+
            kernel[3]+
            kernel[4]+
            kernel[5]+
//            kernel[6]+
//            kernel[7]+
//            kernel[8]+
//            kernel[9]+
            0), 2  // squared
    );


    GRID tmp;

#pragma omp parallel for
    // primo passaggio, kernel verticale
    for( int y=0; y<GRIDH; y++ ){
        for( int x=0; x<GRIDW; x++ ){
            tmp.G[y][x]=
//                kernel[9]*in.WRAP(x,y-9)+
//                kernel[8]*in.WRAP(x,y-8)+
//                kernel[7]*in.WRAP(x,y-7)+
//                kernel[6]*in.WRAP(x,y-6)+
                kernel[5]*in.WRAP(x,y-5)+
                kernel[4]*in.WRAP(x,y-4)+
                kernel[3]*in.WRAP(x,y-3)+
                kernel[2]*in.WRAP(x,y-2)+
                kernel[1]*in.WRAP(x,y-1)+
                kernel[0]*in.WRAP(x,y  )+
                kernel[1]*in.WRAP(x,y+1)+
                kernel[2]*in.WRAP(x,y+2)+
                kernel[3]*in.WRAP(x,y+3)+
                kernel[4]*in.WRAP(x,y+4)+
                kernel[5]*in.WRAP(x,y+5)+
//                kernel[6]*in.WRAP(x,y+6)+
//                kernel[7]*in.WRAP(x,y+7)+
//                kernel[8]*in.WRAP(x,y+8)+
//                kernel[9]*in.WRAP(x,y+9)+
                0;
        }
    }

#pragma omp parallel for
    // secondo passaggio, kernel orizzontale
    for( int x=0; x<GRIDW; x++ ){
        for( int y=0; y<GRIDH; y++ ){
            out.G[y][x] =
//                kernel[9]*tmp.WRAP(x-9,y)+
//                kernel[8]*tmp.WRAP(x-8,y)+
//                kernel[7]*tmp.WRAP(x-7,y)+
//                kernel[6]*tmp.WRAP(x-6,y)+
                kernel[5]*tmp.WRAP(x-5,y)+
                kernel[4]*tmp.WRAP(x-4,y)+
                kernel[3]*tmp.WRAP(x-3,y)+
                kernel[2]*tmp.WRAP(x-2,y)+
                kernel[1]*tmp.WRAP(x-1,y)+
                kernel[0]*tmp.WRAP(x  ,y)+
                kernel[1]*tmp.WRAP(x+1,y)+
                kernel[2]*tmp.WRAP(x+2,y)+
                kernel[3]*tmp.WRAP(x+3,y)+
                kernel[4]*tmp.WRAP(x+4,y)+
                kernel[5]*tmp.WRAP(x+5,y)+
//                kernel[6]*tmp.WRAP(x+6,y)+
//                kernel[7]*tmp.WRAP(x+7,y)+
//                kernel[8]*tmp.WRAP(x+8,y)+
//                kernel[9]*tmp.WRAP(x+9,y)+
                0;
                
            out.G[y][x] *= one_over_sum;
        }
    }
}



