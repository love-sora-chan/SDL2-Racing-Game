#include <SDL.h>
#include <stdio.h>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* window;
SDL_Surface* screen = NULL;
SDL_Texture* texture = NULL;
SDL_Renderer *renderer = NULL;

bool init(){
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	/*else
	{
		//Create window
		window = SDL_CreateWindow( "test1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			screen = SDL_GetWindowSurface( window );

			//Get Render
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		}
	}*/
	return success;
}

bool loadmedia(){

    bool success = 1;

    return success; 
}

void SetPixel(SDL_Surface * surface, int x,int y, uint8_t R, uint8_t G, uint8_t B){
	SDL_LockSurface(surface);
	//SDL_memset(screen->pixels,255,screen->h * screen->pitch);
	uint8_t* pixelArray = (uint8_t*) surface->pixels;
	pixelArray[y*surface->pitch +x*surface->format->BytesPerPixel + 0] = R;
	pixelArray[y*surface->pitch +x*surface->format->BytesPerPixel + 1] = G;
	pixelArray[y*surface->pitch +x*surface->format->BytesPerPixel + 2] = B;
	SDL_UnlockSurface(surface);
}

void close(){

}

int WinMain( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadmedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
            window = SDL_CreateWindow("test2",20,20,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
            screen = SDL_GetWindowSurface(window);
                        
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			int mx,my;
			Uint32 buttons = SDL_GetMouseState(&mx,&my);

			
			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					else if( e.button.button == SDL_BUTTON_LEFT){
						buttons = SDL_GetMouseState(&mx,&my);
						SetPixel(screen, mx, my, 255 ,255,255 );
						
						std::cout<<"left mouse clicked "<<mx<<' '<<my<<'\n';
					}
		
				}
				SDL_UpdateWindowSurface(window);

                

			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}