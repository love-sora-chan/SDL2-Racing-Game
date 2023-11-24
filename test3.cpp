#include <iostream>
#include <SDL.h>
#include <cmath>
#include <algorithm>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int segement_length = 480;
int road_width = 360;

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;


class 3DLine{
    private :
        double x,y,z; //3D center coordinate of line
        int screenX,screenY,width; //line coordinate on screen 
    public:
        3DLine(){
            x=0;
            y=0;
            z=0;
        }
        //3D coordinate to screen coordinate
        void project(int camx, int camy, int camz){
            double scale = camD/(z-camz);
            screenX = (int) (1+scale*(x-camx))*SCREEN_WIDTH/2;
            screenY = (int) (1+scale*(y-camy))*SCREEN_HEIGHT/2;
            width = (int) scale * road_width * width / 2;
        }
};

bool intit(){
    //Initialization flag
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO) < ){
        std::cout<<"SDL couldn't Initialize, Error : "<<SDL_GetError();
        success = false;
    }
    else{
        //Create 
    }
}



//(x1,y1)corresponds to the coordinate of the line center on screen, w1 is the width of line
void draw_quad(SDL_Renderer * renderer, int x1, int y1, int w1, int x2, int y2, int w2, int R, int G , int B , int A){
    if(y1>y2){
        std::swap(y1,y2);
        std::swap(x1,x2);
        std::swap(w1,w2);
    }
    SDL_SetRenderDrawColor(renderer, R,G,B,A);
    for(int y = y1 ; y <= y2 ; y++){
        int w = w1 + (w2 - w1) * (y - y1) / (y2 - y1) ;
        int xmid = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        SDL_RenderDrawLine(renderer, xmid - (w/2) , y, xmid + (w/2), y);
    }
}

int WinMain(){





}

