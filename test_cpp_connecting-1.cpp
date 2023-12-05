#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>

#include "LTexture.cpp"
#include "Record.cpp"
#include "MENU.cpp"
#include "finishPage.cpp"
#include "name_insert_page.cpp"
#include "test4-2.cpp"

SDL_Window* gWindow = NULL;
SDL_Texture* gTexture = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* Font = NULL;
TTF_Font* Font2 = NULL;
Mix_Music* gMusic = NULL;

Mix_Chunk* button = NULL;
Mix_Chunk* changePageSound = NULL;




//SDL_Color White= {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};

enum current_page{
	Home_page = 0,
	Map_Page,
	Record_Page,
    Finish_Page,
    Game_Page,
    InsertName_Page,

};
current_page STATUS = Home_page;
/*const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;*/

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
	
    button = Mix_LoadWAV("testmusic/change_button.wav");
    changePageSound = Mix_LoadWAV("testmusic/button_enter.wav");
	
}








void Transition(SDL_Renderer* REND){
    SDL_RenderClear(REND);
	LTexture load_pic;
	load_pic.loadFromFile("testimage/loading.png",REND);
	load_pic.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
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
			//rec.RecordReset(rec,"TEST");
            rec.loadFromFile(rec);

            finishPage finish_page;
            finish_page.Initialize(gRenderer);
            finish_page.setUsedTime(finish_page,60.023);

			MENU main_menu;
			main_menu.Initialize(main_menu,gRenderer);
			main_menu.choosing(main_menu.opt,main_menu,gRenderer);
			main_menu.show(main_menu,gRenderer);

            INP inp;


			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//check if game is running
			bool in_play = 0;

			//While application is running
			while( !quit )
			{

				Uint32 game_start_time;
            	bool game_started = 0;
				//Handle events on queue
				while( SDL_PollEvent( &e ) )
				{
					//calculate time
					Uint32 time_start = SDL_GetTicks();

					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					
					else if ((int)STATUS < 4){
                        
                        if(e.type == SDL_KEYDOWN){
						switch((int)STATUS){
							case 0:{
                                {
								switch(e.key.keysym.sym){
									case SDLK_UP:{
										SDL_RenderClear(gRenderer);
                                        Mix_PlayChannel(-1,button,0);                                       
										main_menu.UP_opt(main_menu);                                                            
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.show(main_menu,gRenderer);
										break;
									}
									case SDLK_DOWN:{
										SDL_RenderClear(gRenderer);
                                        Mix_PlayChannel(-1,button,0);
										main_menu.DOWN_opt(main_menu);
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.show(main_menu,gRenderer);
										break;
									}
									case SDLK_RETURN:{										
                                        switch(main_menu.opt){
                                            case 1:{
                                                /*STATUS = Finish_Page;
                                                Mix_PlayChannel(-1,changePageSound,0);
                                                Transition(gRenderer);
                                                finish_page.Initialize(gRenderer);
                                                finish_page.show(finish_page,gRenderer,Font,0);
												*/
												STATUS = Game_Page;
                                                break;
                                            }
                                            case 2:{
                                                STATUS = Record_Page;
                                                Mix_PlayChannel(-1,changePageSound,0);
											    Transition(gRenderer);
											    rec.show(rec,gRenderer,Font,Font2);
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
                            switch(e.key.keysym.sym){
                                case SDLK_ESCAPE:{
                                    STATUS = Home_page;
									Transition(gRenderer);
									main_menu.Initialize(main_menu,gRenderer);
									main_menu.choosing(main_menu.opt,main_menu,gRenderer);
									main_menu.appear(main_menu,gRenderer);
									break;
                                }
                                break;                                							
							}
                            break;					
							}
                            case 3:{
                                switch(e.key.keysym.sym){
                                    case SDLK_ESCAPE:{
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
                                    case SDLK_RETURN:{
                                        SDL_RenderClear(gRenderer);
                                        Mix_PlayChannel(-1,changePageSound,0);
                                        Transition(gRenderer);
                                        inp.Initialize(inp,gRenderer);
                                        inp.show_centered(inp,gRenderer);
                                        STATUS = InsertName_Page;                                                                             
                                        break;
                                    }
                                break;
                                }
                            break;
                            }
                            
						}
                    }}
					else if(STATUS == Game_Page){
						if(in_play==0){
							load_game_media(gRenderer);

							create_map(0);
							create_car();
							create_camera();


							in_play = 1;
						}


						if(keyarr[SDL_SCANCODE_LEFT])
							car_main->move_left();
						if(keyarr[SDL_SCANCODE_RIGHT])
							car_main->move_right();
						if(keyarr[SDL_SCANCODE_UP]){
							car_main->accelerate();
							if(game_started==0){
								game_start_time = SDL_GetTicks();
								game_started=1;
							}
							if(Mix_Paused(0) == 1 || Mix_Playing(0) == 0){
								if(car_main->get_speed_vz()>500){
									Mix_PlayChannel(0,accelerating_fast,0);
								}
								else{
									Mix_PlayChannel(0,accelerating,0);
								}
							}
						}
						else{
							if(Mix_Playing(0) != 0){
								Mix_Pause(0);
							}
						}
							
						if(keyarr[SDL_SCANCODE_DOWN]){
							car_main->decelerate();
							if(  Mix_Paused(1) == 1 || Mix_Playing(1) == 0 ){
								Mix_PlayChannel(1,decelerating,0);
							}
							if( car_main->get_speed_vz() <= 1 ){
								Mix_Pause(1);
							}
						}
						else{
							if(Mix_Playing(1) != 0){
								Mix_Pause(1);
							}
						}
						//car turn due to road curve
						car_main->turn(map);

						//Test
						SDL_SetRenderDrawColor(gRenderer, Blue_Sky.r,Blue_Sky.g,Blue_Sky.b,Blue_Sky.a);
						SDL_RenderClear(gRenderer);

						//Draw Scene
						draw_scene(gRenderer, map, cam ,300);

						//draw speed
						show_speed(gRenderer, car_main);

						//draw progress
						show_percentage(gRenderer, map, car_main);

						//show_time(game_start_time,SDL_GetTicks(),game_started);           
						
						//draw cars, main car
						draw_cars(gRenderer, cam , car_main);

						//Check collision between obstacle and car
						Car_Obstacle_Collision(car_main,map,30);
						
						//check if fell into ocean
						Fell_into_Ocean(car_main, map);

						if(car_main->is_car_intact() == 0){
							close_game();
							STATUS = Home_page;
							in_play = 0;
						}
						//check if passed finish line
						if(Reach_Finish(car_main, map)){
							close_game();
							STATUS = Finish_Page;
							in_play = 0;
						}

						//Present
						SDL_RenderPresent(gRenderer);
						
					}
                    else if((int)STATUS == 5){
                        if(e.key.keysym.sym == SDLK_RETURN){
                            if(inp.name.length()==0)
                                inp.name = "UNKNOWN";

                            static double n = 30.1;
                            rec.setRecordTime(rec,n);
                            rec.setRecordName(rec,inp.name);
                            n -= 3.25;

                            rec.writeToFile(rec);

                            STATUS = Home_page;
                            Mix_PlayChannel(-1,changePageSound,0);
							Transition(gRenderer);
							main_menu.Initialize(main_menu,gRenderer);
							main_menu.choosing(main_menu.opt,main_menu,gRenderer);
							main_menu.appear(main_menu,gRenderer);
							break;
                        }
                        inp.handle_input(inp,gRenderer,e);
                    }
					


					framerate_cap(time_start, 60);
				}
				
				if(STATUS != Game_Page){
					SDL_UpdateWindowSurface(gWindow);
				}
				

				
		}
		SDL_DestroyRenderer(gRenderer);
	}
	SDL_DestroyWindow(gWindow);
	
	}
	return 0;
}