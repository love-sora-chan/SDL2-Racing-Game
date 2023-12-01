#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cmath>
#include <algorithm>
#include <vector>
//#include <SDL_bgi.h>


const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 720;

int segement_length = 200;
int road_width = 2000;

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
SDL_Color Blue_Sky= {.r = 53, .g = 81, .b = 92, .a = SDL_ALPHA_OPAQUE};
SDL_Color Water= {.r = 15, .g = 94, .b = 156, .a = SDL_ALPHA_OPAQUE};



class Camera3D{
    public:
        double x, y ,z, D, vz, vx;
        Camera3D(){
            x=0, y=1000, z=0, D=0.84, vx = 0, vz = 0;
        }
        Camera3D(double x, double y, double z, double D, double vx,double vz){
            this->x=x, this->y=y, this->z=z, this->D=D, this->vx=vx, this->vz=vz;
        }
};

class Line3D{
    public :
        double x,y,z; //3D center coordinate of line
        int screenX,screenY,width; //line coordinate on screen 
        double scale, curve;

        Line3D(){
            x=0, y=0, z=0, scale=0, curve=0;
        }
        //3D coordinate to screen coordinate
        void project(Camera3D * cam){
            double delta_z = (z - cam->z == 0.0 ? 1 : z - cam->z);
            scale = cam->D/(z-cam->z);
            screenX = (int) ((1 + scale*( x - cam->x ))*SCREEN_WIDTH / 2);
            screenY = (int) ((1 - scale*( y - cam->y ))*SCREEN_HEIGHT / 2);
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
    float p1x = (float) ( x1-w1>0 ? x1-w1 : 0 );
    float p1y = (float) y1;
    float p2x = (float) ( x1+w1<SCREEN_WIDTH ? x1+w1 : SCREEN_WIDTH - 1 );
    float p2y = (float) y1;
    float p3x = (float) ( x2+w2<SCREEN_WIDTH ? x2+w2 : SCREEN_WIDTH - 1 );
    float p3y = (float) y2;
    float p4x = (float) ( x2-w2>0 ? x2-w2 : 0 );
    float p4y = (float) y2;
    if(y2==y1){
        SDL_SetRenderDrawColor(renderer, color.r, color.g,color.b,color.a);
        SDL_RenderDrawLine(renderer, x1,y1,x2,y2);
    }
    else if(y2-y1 >= 1 || y2-y1 <= -1){
        std::vector< SDL_Vertex > verts1 =
        {
            { SDL_FPoint{ p1x, p1y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ p2x, p2y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ p3x, p3y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
        };
        SDL_RenderGeometry(renderer, NULL,verts1.data(), verts1.size(), NULL,0 );
        std::vector< SDL_Vertex > verts2 =
        {
            { SDL_FPoint{ p1x, p1y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ p3x, p3y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ p4x, p4y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
        };
        SDL_RenderGeometry(renderer, NULL,verts2.data(), verts2.size(), NULL,0 );        
    }

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

void draw_scene(SDL_Renderer * renderer, Line3D * lines, Camera3D * cam, int lines_drawn, int total_lines){
    int start_pos = cam->z / segement_length;
    double x = 0, dx = 0;
    for(int i = 1+start_pos ; i < lines_drawn+start_pos ; i++){
        Line3D * curr_line = &lines[i % total_lines];
        Line3D * prev_line = &lines[(i-1) % total_lines];
        cam->x -= x;
        curr_line->project(cam);
        cam->x += x;
        x+=dx; dx+=curr_line->curve;

        SDL_Color grass = (i/3)%2 ? Light_Green : Dark_Green;
        SDL_Color rumble = (i/3)%2 ? White : Black;
        SDL_Color road = (i/3)%2 ? Light_Road : Dark_Road;


        draw_quad(renderer, 0, prev_line->screenY, SCREEN_WIDTH, 0,curr_line->screenY, SCREEN_WIDTH, Water);
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width*1.2 , curr_line->screenX ,curr_line->screenY, curr_line->width*1.2 , rumble);
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width , curr_line->screenX ,curr_line->screenY, curr_line->width , road);
        //draw_quad(renderer, 50,50,50,200,200,100,Light_Road);
        draw_quad(renderer, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, Blue_Sky);

    }
    cam->z+=cam->vz;
    //cam->x -= lines[1+start_pos].curve*25;
    //cam->x+=cam->vx;    
    //circle(1,1,1);
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
            int N = 50000 ;
            Line3D * Line3DArray = new Line3D[N];
            for(int i = 0 ; i < N ; i++){
                Line3D line;
                Line3DArray[i] = line;
                Line3DArray[i].z = segement_length * i;
            }
            for(int i = 100 ; i < 300 ; i++){
                //Line3DArray[i].curve = 0.5;
                Line3DArray[i].curve = 0.5;
            }
            for(int i = 500 ; i < 1000 ; i++){
                //Line3DArray[i].curve = 0.5;
                Line3DArray[i].curve = -0.7;
            }

            for(int i = 1000 ; i < 1200 ; i++){
                //Line3DArray[i].curve = 0.5;
                Line3DArray[i].curve = 1;
            }

            for(int i = 1400 ; i < 1700 ; i++){
                //Line3DArray[i].curve = 0.5;
                Line3DArray[i].curve = -0.8;
            }

            //Create Camera
            Camera3D * cam = new Camera3D(0,2500,0,0.84,0,0);



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
                                cam->vz+=10;
							    break;

							case SDLK_DOWN:
                                cam->vz-=10;
                                cam->vz = ( cam->vz>0 ? cam->vz : 0);
							    break;

							case SDLK_LEFT:
                                //cam->vx-=10;
                                cam->x -= 30;
							    break;

							case SDLK_RIGHT:
                                //cam->vx+=10;
                                cam->x += 30;
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
                draw_scene(renderer, Line3DArray, cam ,300,N);
                

                //Present
                SDL_RenderPresent(renderer);
                
                
                //cap framerate
                //framerate_cap(time_start, 1000);

                
            }
            delete [] Line3DArray;
            delete cam;
            
        }
    }
    close();


    return 0;
}

