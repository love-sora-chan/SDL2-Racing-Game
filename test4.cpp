#include <iostream>
#include <SDL.h>
//#include <SDL2_gfxPrimitives.h>
#include <cmath>
#include <algorithm>
#include <vector>
//#include <SDL2_gfx>
/*extern "C" {
    #include "src\gfx\SDL2_gfxPrimitives.c"
}*/
#include <SDL2_gfxPrimitives.h>


const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

int segement_length = 200;
int road_width = 2000;
double camD = 0.84; //Camera Depth

SDL_Window * window = NULL;
SDL_Surface * surface = NULL;
SDL_Texture * texture = NULL;
SDL_Renderer * renderer = NULL;

SDL_Color White= {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};
SDL_Color Black= {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Dark_Green= {.r = 0, .g = 102, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Light_Green= {.r = 0, .g = 255, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Dark_Road= {.r = 100, .g = 100, .b = 100, .a = SDL_ALPHA_OPAQUE};
SDL_Color Light_Road= {.r = 105, .g = 105, .b = 105, .a = SDL_ALPHA_OPAQUE};


class Line3D{
    public :
        double x,y,z; //3D center coordinate of line
        int screenX,screenY,width; //line coordinate on screen 

        Line3D(){
            x=0;
            y=0;
            z=0;
        }
        //3D coordinate to screen coordinate
        void project(int camx, int camy, int camz){
            double scale = camD/(z-camz);
            screenX = (int) ((1 + scale*( x - camx))*SCREEN_WIDTH/2);
            screenY = (int) ((1 - scale*( y - camy))*SCREEN_HEIGHT/2);
            width = (int) (scale * road_width * SCREEN_WIDTH / 2);
        }
};

bool init(){
    //Initialization flag
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO) < 0){
        std::cout<<"SDL couldn't Initialize, Error : "<<SDL_GetError();
        success = false;
    }
    else{
        //Create window
        window = SDL_CreateWindow( "test4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			//surface = SDL_GetWindowSurface( window );

			//Get Render
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		}
    }
    return success;
}

bool loadmedia(){
    bool success = 1;
    return success; 
}

void close(){
    SDL_DestroyRenderer(renderer);
    //SDL_DestroyWindowSurface(window);
    SDL_DestroyWindow(window);
}

//(x1,y1)corresponds to the coordinate of the line center on screen, w1 is the width of line
void draw_quad(SDL_Renderer * renderer, int x1, int y1, int w1, int x2, int y2, int w2, SDL_Color color){
    if(y1>y2){
        std::swap(y1,y2);
        std::swap(x1,x2);
        std::swap(w1,w2);
    }

    //else{
        std::vector< SDL_Vertex > verts1 =
        {
            { SDL_FPoint{ x1-w1, y1 }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ x1+w1, y1 }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ x2-w2, y2 }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
        };
        SDL_RenderGeometry(renderer, NULL,verts1.data(), verts1.size(), NULL,0 );
        std::vector< SDL_Vertex > verts2 =
        {
            { SDL_FPoint{ x2-w2, y2 }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ x1+w1, y1 }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ x2+w2, y2 }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
        };
        SDL_RenderGeometry(renderer, NULL,verts2.data(), verts2.size(), NULL,0 );


    //}
/*
    std::vector< SDL_Vertex > verts =
    {
        { SDL_FPoint{ 100, 100 }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ 450, 450 }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ 300,  450}, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ 150, 150 }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ 0 }, },
    };
    SDL_RenderGeometry(renderer, NULL,verts.data(), verts.size(), NULL,0 ); 
*/
    /*
    if(y1>y2){
        std::swap(y1,y2);
        std::swap(x1,x2);
        std::swap(w1,w2);
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    if(y1==y2){
        //SDL_RenderDrawLine(renderer, x1 , y1 , x2, y2 );
    }
    else{
        for(int y = y1 ; y <= y2 ; y++){
            int w = w1 + (w2 - w1) * (y - y1) / (y2 - y1) ;
            int xmid = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            SDL_RenderDrawLine(renderer, xmid - w , y, xmid + w, y);
        }        
    }
    */
}

void draw_scene(SDL_Renderer * renderer, Line3D * lines, int camZ, int n, int N){
    int start_pos = camZ / segement_length;
    for(int i = 1+start_pos ; i < n+start_pos ; i++){
        Line3D * curr_line = &lines[i%N];
        Line3D * prev_line = &lines[(i-1)%N];
        prev_line->project(1,1500,camZ);
        curr_line->project(1,1500,camZ);

        SDL_Color grass = (i/3)%2 ? Light_Green : Dark_Green;
        SDL_Color rumble = (i/3)%2 ? White : Black;
        SDL_Color road = (i/3)%2 ? Light_Road : Dark_Road;

        //std::cout<<prev_line->screenX<<' '<<prev_line->screenY<<'\n';

        draw_quad(renderer, 0, prev_line->screenY, SCREEN_WIDTH, 0,curr_line->screenY, SCREEN_WIDTH, grass);
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width*1.2 , curr_line->screenX ,curr_line->screenY, curr_line->width*1.2 , rumble);
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width , curr_line->screenX ,curr_line->screenY, curr_line->width , road);
        //draw_quad(renderer, 50,50,50,200,200,100,Light_Road);

    
    
        
        
    }
}

void framerate_cap(Uint32 start, int fps){
    double ms_per_frame = 1000/fps;
    double elapsed_time = (double) (SDL_GetTicks() - start);
    if( elapsed_time < ms_per_frame ){
        SDL_Delay( (int) (ms_per_frame - elapsed_time) );
    }
    //std::cout<<ms_per_frame<<' ';
}
int WinMain(){

    if( !init() ){
        std::cout<<"Failed to Initialize\n";
    }
    else{
        if( !loadmedia() ){
            std::cout<<"Failed to load media\n";
        }
        else{
            bool quit = false;

            //Event Handler
            SDL_Event e;

            //Create Lines
            int N = 1600 ,camZ = 1;
            Line3D * Line3DArray = new Line3D[N];
            for(int i = 0 ; i < N ; i++){
                Line3D line;
                Line3DArray[i] = line;
                Line3DArray[i].z = segement_length * i;
            }


            //Clear renderer
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            //While app is running
            while( !quit ){
                //calculate time
                Uint32 time_start = SDL_GetTicks();





                //Handle event on queue
                while( SDL_PollEvent( &e ) != 0){
                    if( e.type == SDL_QUIT ){
                        quit = true;
                    }
                    else if(e.type == SDL_KEYDOWN){
                        switch( e.key.keysym.sym )
						{
							case SDLK_UP:
                                camZ+=500;
							break;

							case SDLK_DOWN:
                                camZ-=500;
							break;

							case SDLK_LEFT:
							
							break;

							case SDLK_RIGHT:
							
							break;

							default:
							
							break;
						}
                    }
                    else{
                        //do keyboard or mouse interaction event
                    }
                }
                

                //Test
                SDL_SetRenderDrawColor(renderer, 255,255,255,255);
                //SDL_RenderDrawLine(renderer, 0,0,100,100);
                //draw_quad(renderer, 0,100,100,500,500,200,Light_Green);
                

                //Draw Scene
                draw_scene(renderer, Line3DArray, camZ ,300,N);
                

                //Present
                SDL_RenderPresent(renderer);
                
                
                //cap framerate
                framerate_cap(time_start, 1000);
                //std::cout<<"haha\n";

                
            }
            delete [] Line3DArray;
            //delete Line3DArray;
            
        }
    }
    close();


    return 0;
}

