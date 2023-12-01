#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "LTexture.cpp"
#include "Record.cpp"
#include "MENU.cpp"
#include "finishPage.cpp"

SDL_Window* gWindow = NULL;
SDL_Texture* gTexture = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* Font = NULL;

SDL_Color White= {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};

enum current_page{
	Home_page = 0,
	Map_Page,
	Record_Page,
    Finish_Page,
};
current_page STATUS = Home_page;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 750;

void InitializeSDL(){
	//Initialize SDL
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	
	//Initialize SDL_image
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP | IMG_INIT_JXL | IMG_INIT_AVIF;
	if( !( IMG_Init( imgFlags ) & imgFlags ))
	{
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
	}
	
	//Initialize SDL_ttf
	if( TTF_Init() == -1 )
	{
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	//Initialize SDL_mixer
	int mixFlags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_OPUS | MIX_INIT_FLAC;
	if( !( Mix_Init( mixFlags ) & mixFlags ))
	{
		printf("SDL_mix could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}
	//open audio devices
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
	    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
	}
	
	
}








void Transition(SDL_Renderer* REND){
    SDL_RenderClear(REND);
	LTexture load_pic;
	load_pic.loadFromFile("testimage/load.png",REND);
	load_pic.render(SCREEN_WIDTH/5*2,SCREEN_HEIGHT/2,SCREEN_WIDTH/5,SCREEN_HEIGHT/5,REND);
	SDL_RenderPresent(REND);										
	SDL_Delay(1000);
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);
}




int WinMain(int argc,char *argv[]){
	InitializeSDL();
	gWindow = SDL_CreateWindow( "TESTING", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	//You'd better check if it success!
	if(gWindow == NULL){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError() );
	}else{
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(gRenderer == NULL){
			printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else{
			
			Record rec;
			rec.RecordReset(rec);

            finishPage finish_page;
            finish_page.Initialize(gRenderer);
            finish_page.setUsedTime(finish_page,60.023);


			MENU main_menu;
			main_menu.Initialize(main_menu,gRenderer);
			main_menu.choosing(main_menu.opt,main_menu,gRenderer);
			main_menu.show(main_menu,gRenderer);


			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					
					else if ((int)STATUS != 4){
                        if(e.type == SDL_KEYDOWN){
						switch((int)STATUS){
							case 0:{
                                {
								switch(e.key.keysym.sym){
									case SDLK_UP:{
										SDL_RenderClear(gRenderer);
										main_menu.UP_opt(main_menu);
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.show(main_menu,gRenderer);
										break;
									}
									case SDLK_DOWN:{
										SDL_RenderClear(gRenderer);
										main_menu.DOWN_opt(main_menu);
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.show(main_menu,gRenderer);
										break;
									}
									case SDLK_RETURN:{										
                                        switch(main_menu.opt){
                                            case 1:{
                                                STATUS = Finish_Page;
                                                Transition(gRenderer);
                                                finish_page.Initialize(gRenderer);
                                                finish_page.show(finish_page,gRenderer,Font,0);
                                                break;
                                            }
                                            case 2:{
                                                STATUS = Record_Page;
											    Transition(gRenderer);
											    rec.show(rec,gRenderer,Font);
                                                break;
                                            }
                                            case 3:{
                                                quit = true;
                                                break;
                                            }                                       
                                        }
                                        break;	}								
									}									
									}
								break;
							}
							case 2:{                                							
                                if(e.key.keysym.sym==SDLK_b){									
                                    STATUS = Home_page;
									Transition(gRenderer);
									main_menu.Initialize(main_menu,gRenderer);
									main_menu.choosing(main_menu.opt,main_menu,gRenderer);
									main_menu.appear(main_menu,gRenderer);
									break;								
								}						
							}
                            case 3:{
                                switch(e.key.keysym.sym){
                                    case SDLK_b:{
                                        STATUS = Home_page;
									    Transition(gRenderer);
									    main_menu.Initialize(main_menu,gRenderer);
									    main_menu.choosing(main_menu.opt,main_menu,gRenderer);
									    main_menu.appear(main_menu,gRenderer);
									    break;
                                    }
                                    case SDLK_f:{
                                        finish_page.show(finish_page,gRenderer,Font,1);
                                        break;
                                    }
                                    case SDLK_s:{
                                        finish_page.show(finish_page,gRenderer,Font,0);
                                        break;
                                    }
                                }
                            }
						}}
						
					}
				}
				SDL_UpdateWindowSurface(gWindow);
		}
		SDL_DestroyRenderer(gRenderer);
	}
	SDL_DestroyWindow(gWindow);
	return 0;
	}
}