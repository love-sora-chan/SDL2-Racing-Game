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
#include "Map_Page.cpp"
#include "RECORD_PAGE.cpp"
#include "finishPage.cpp"
#include "name_insert_page.cpp"
#include "Game.cpp"

SDL_Window* gWindow = NULL;
SDL_Texture* gTexture = NULL;
SDL_Renderer* gRenderer = NULL;
Mix_Music* gMusic = NULL;

Mix_Chunk* buttonSound = NULL;
Mix_Chunk* changePageSound = NULL;
Mix_Chunk* backSound = NULL;
Mix_Chunk* textsound = NULL;
Mix_Chunk* countdown = NULL;
Mix_Chunk* crashed = NULL;
Mix_Chunk* dropped = NULL;

SDL_Color White= {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};

enum current_page{
	Home_page = 0,
	Map_Page,
	Record_Page,
    Finish_Page,
    Game_Page,
    InsertName_Page,
};
current_page STATUS = Home_page;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

Car_Type car_parameter;
Map_Type map_parameter;
Difficulty difficulty_parameter;
Car_Status carintact;


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
	
    buttonSound = Mix_LoadWAV("audio/change_button.wav");
    changePageSound = Mix_LoadWAV("audio/button_enter.wav");
	backSound = Mix_LoadWAV("audio/escape_button.wav");
	textsound = Mix_LoadWAV("audio/text_input.wav");
	countdown = Mix_LoadWAV("audio/countdown.wav");
	crashed = Mix_LoadWAV("audio/crash.wav");
	dropped = Mix_LoadWAV("audio/fall in water.wav");

	gMusic = Mix_LoadMUS("audio/beat of the rising sun.mp3");
}








void Transition(SDL_Renderer* REND,int delaytime = 1000){
    SDL_SetRenderDrawColor(REND,0,0,0,255);
	SDL_RenderClear(REND);
	if(delaytime>0){
		LTexture load_pic;
		load_pic.loadFromFile("image/menu/loading.png",REND);
		load_pic.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
		SDL_RenderPresent(REND);
	}										
	SDL_Delay(delaytime);
	if(delaytime>0){
		SDL_PumpEvents();
		SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);
	}
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
			LTexture login;
			login.loadFromFile("image/menu/main_menu.png",gRenderer);
			SDL_Delay(3000);
			for(int a = 256;a > 128;a -= 4){
				SDL_Delay(10);
				SDL_RenderClear(gRenderer);
				login.setAlpha(a);
				login.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,gRenderer);
				SDL_RenderPresent(gRenderer);
			}
			login.free();
			
			RecordPage rp;
			//rp.allreset(rp);
			rp.read(rp);

            finishPage finish_page;
            finish_page.Initialize(gRenderer);

			MENU main_menu;
			main_menu.Initialize(main_menu,gRenderer);
			main_menu.choosing(main_menu.opt,main_menu,gRenderer);
			main_menu.show(main_menu,gRenderer);

            INP inp;

			MapPage mp;
			mp.Initialize();

			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//check if game is running
			bool in_play = 0;

			//While application is running
			while( !quit )
			{
				Mix_VolumeMusic(64);
				Uint32 game_start_time;
            	bool game_started = 0;
				if(Mix_PlayingMusic()==0 && main_menu.PlayMusic()){
					Mix_PlayMusic(gMusic,0);
				}
				Mix_Volume(-1,192);
				//Handle events on queue
				while( SDL_PollEvent( &e ) )
				{
					

					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					
					else if ((int)STATUS < 3){
                        
                        if(e.type == SDL_KEYDOWN){
						switch(STATUS){
							case Home_page:{
                                {
								switch(e.key.keysym.sym){
									case SDLK_UP:{
										SDL_RenderClear(gRenderer);
                                        Mix_PlayChannel(-1,buttonSound,0);                                       
										main_menu.UP_opt(main_menu);                                                            
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.show(main_menu,gRenderer);
										break;
									}
									case SDLK_DOWN:{
										SDL_RenderClear(gRenderer);
                                        Mix_PlayChannel(-1,buttonSound,0);
										main_menu.DOWN_opt(main_menu);
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.show(main_menu,gRenderer);
										break;
									}
									case SDLK_RETURN:{										
                                        switch(main_menu.opt){
                                            case 1:{                                                
												STATUS = Map_Page;
												Mix_PlayChannel(-1,changePageSound,0);
												mp.show(mp,gRenderer);
                                                break;
                                            }
                                            case 2:{
                                                STATUS = Record_Page;
                                                Mix_PlayChannel(-1,changePageSound,0);
											    Transition(gRenderer);
											    rp.show(rp,gRenderer);
                                                break;
                                            }
                                            case 3:{ 
												Mix_HaltMusic();                                              
                                                quit = true;
                                                break;
                                            }                                       
                                        }
                                        break;	}								
									}									
									}
								break;
							}
							case Map_Page:{
								switch(e.key.keysym.sym){
									case SDLK_UP:mp.UP_opt(mp);break;
									case SDLK_DOWN:mp.DOWN_opt(mp);break;
									case SDLK_LEFT:mp.left_opt(mp);break;
									case SDLK_RIGHT:mp.right_opt(mp);break;
									case SDLK_RETURN:{
										if(mp.ifMapChosen()){
											STATUS = Game_Page;
											Mix_HaltMusic();
											main_menu.setPlayMusic(false);
											map_parameter = (Map_Type)mp.getMap();
											car_parameter = (Car_Type)mp.getCar();
											difficulty_parameter = (Difficulty)mp.getDifficulty();
										}
										else{}
										Mix_PlayChannel(-1,changePageSound,0);
										mp.switchPage();
										break;
									}
									case SDLK_ESCAPE:{
										if(mp.ifMapChosen()){mp.switchPage();Mix_PlayChannel(-1,backSound,0);}
										else{
											STATUS = Home_page;
											Mix_PlayChannel(-1,backSound,0);										
											Transition(gRenderer);
											mp.Initialize();											
											main_menu.Initialize(main_menu,gRenderer);
											main_menu.choosing(main_menu.opt,main_menu,gRenderer);
											main_menu.appear(main_menu,gRenderer);
										}										
										break;
									}
								}
								if(STATUS == Map_Page)mp.show(mp,gRenderer);
								break;
							}
							case Record_Page:{                                							
                            switch(e.key.keysym.sym){
                                case SDLK_ESCAPE:{
                                    STATUS = Home_page;
									Mix_PlayChannel(-1,backSound,0);
									Transition(gRenderer);
									main_menu.Initialize(main_menu,gRenderer);
									main_menu.choosing(main_menu.opt,main_menu,gRenderer);
									main_menu.appear(main_menu,gRenderer);
									break;
                                }
								case SDLK_LEFT:{
									rp.left_opt(rp);
									rp.show(rp,gRenderer);
									break;
								}
								case SDLK_RIGHT:{
									rp.right_opt(rp);
									rp.show(rp,gRenderer);
									break;
								}
								case SDLK_UP:{
									rp.UP_DOWN_opt(rp);
									rp.show(rp,gRenderer);
									break;
								}
								case SDLK_DOWN:{
									rp.UP_DOWN_opt(rp);
									rp.show(rp,gRenderer);
									break;
								}
                                break;                                							
							}
                            break;					
							}
                            
                            
						}
                    }}
					else if(STATUS==Finish_Page){
                                
                                    if(carintact == Intact){
										
										SDL_Delay(3000);
                                        Transition(gRenderer);
                                        inp.Initialize(inp,gRenderer);
                                        inp.show_centered(inp,gRenderer);
                                        STATUS = InsertName_Page;  
										SDL_PumpEvents();
										SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);                                                                           
                                        break;
									}
									else{
										if(carintact == In_Water)	Mix_PlayChannel(-1,dropped,0);
										else Mix_PlayChannel(-1,crashed,0);
										SDL_Delay(4500);
										STATUS = Home_page;
										carintact = Intact;
										Transition(gRenderer);
										main_menu.Initialize(main_menu,gRenderer);
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.appear(main_menu,gRenderer);
										break;
									}
                                
                    }
					else if(STATUS == Game_Page){
						
						Uint32 elapsed_time = 0;
						if(in_play==0){
							load_game_media(gRenderer,map_parameter,difficulty_parameter);
							create_map(map_parameter,difficulty_parameter,map);
							create_car(car_parameter);
							create_camera();							
							
							SDL_RenderClear(gRenderer);
							draw_words(gRenderer,"Arrow Key UP↑/DOWN↓ to accelerate/decelerate",SCREEN_WIDTH/2,SCREEN_HEIGHT*1/4,50);
							draw_words(gRenderer,"Arrow Key LEFT←/RIGHT→ to go left/right",SCREEN_WIDTH/2,SCREEN_HEIGHT*2/4,50);
							draw_words(gRenderer,"Press Enter to start game",SCREEN_WIDTH/2,SCREEN_HEIGHT*3/4,50);
							SDL_RenderPresent(gRenderer);

							while( !quit && in_play == 0){
								while( SDL_PollEvent( &e ) != 0){
									if( e.type == SDL_QUIT ){
										quit = true;
									}
									if(keyarr[SDL_SCANCODE_RETURN]){
										in_play = 1;
									}
								}
							}

						}

						Mix_VolumeChunk(countdown,32);
						Mix_PlayChannel(-1,countdown,0);
						game_start_time = SDL_GetTicks();
						while(SDL_GetTicks()-game_start_time<4700){
							int time_pass = 4.7 - (SDL_GetTicks()-game_start_time)/1000.0;
							
							draw_scene(gRenderer, map, cam ,car_main, 300);
							draw_cars(gRenderer, cam , car_main);
							draw_words(gRenderer,std::to_string(time_pass),SCREEN_WIDTH/2, SCREEN_HEIGHT/3,200);

							SDL_RenderPresent(gRenderer);
						}
						game_start_time = SDL_GetTicks();
						game_started=1;
						SDL_PumpEvents();
						SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);

						while( !quit && in_play == 1){
							//calculate time
							Uint32 time_start = SDL_GetTicks();
							while( SDL_PollEvent( &e ) != 0){
									if( e.type == SDL_QUIT ){
										quit = true;
									}
							}
							if(keyarr[SDL_SCANCODE_LEFT])
								car_main->move_left();
							if(keyarr[SDL_SCANCODE_RIGHT])
								car_main->move_right();
							if(keyarr[SDL_SCANCODE_UP]){
								car_main->accelerate();

								if(Mix_Paused(0) == 1 || Mix_Playing(0) == 0){
									if(car_main->get_vz()>500){
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
								if( car_main->get_vz() <= 1 ){
									Mix_Pause(1);
								}
							}
							else{
								if(Mix_Playing(1) != 0){
									Mix_Pause(1);
								}
							}
							
							//Draw Scene
							draw_scene(gRenderer, map, cam ,car_main, 300);

							
							
							//car turn due to road curve
							car_main->turn(map);
							//update car position
							car_main->update_position();
							

							//draw speed
							show_speed(gRenderer, car_main);

							//draw progress
							show_percentage(gRenderer, map, car_main);

							elapsed_time = show_time(gRenderer,game_start_time,SDL_GetTicks(),game_started);           
							
							//draw cars, main car
							draw_cars(gRenderer, cam , car_main);
							SDL_RenderPresent(gRenderer);


							//Check collision between obstacle and car
							Car_Obstacle_Collision(car_main,map,25);
							
							//check if fell into ocean
							Fell_into_Ocean(car_main, map);

							//check if pressed esc
							if(keyarr[SDL_SCANCODE_ESCAPE]){
								Mix_HaltMusic();
								Mix_HaltChannel(-1);
								close_game();
								STATUS = Home_page;
								in_play = 0;
								Transition(gRenderer);
								main_menu.Initialize(main_menu,gRenderer);
								main_menu.choosing(main_menu.opt,main_menu,gRenderer);
								main_menu.appear(main_menu,gRenderer);	
							}
							//check if passed finish line
							else if(Reach_Finish(car_main, map)){
								//Mix_HaltMusic();
								Mix_FadeOutMusic(0);
								Mix_HaltChannel(-1);
								carintact = Intact;
								close_game();
								STATUS = Finish_Page;
								in_play = 0;
								finish_page.setUsedTime(finish_page,((double)elapsed_time/1000));
								rp.record[(int)map_parameter][(int)difficulty_parameter].setRecordTime(rp.record[(int)map_parameter][(int)difficulty_parameter],((double)elapsed_time/1000));
								Transition(gRenderer,0);
								finish_page.Initialize(gRenderer);
                                finish_page.show(finish_page,gRenderer,(int)carintact);
								
/*
								Mix_PlayChannel(-1,changePageSound,0);
								Transition(gRenderer);
								inp.Initialize(inp,gRenderer);
								inp.show_centered(inp,gRenderer);
*/

							}
							else if(car_main->is_car_intact() != Intact){
								Mix_HaltMusic();
								Mix_HaltChannel(-1);
								carintact = car_main->is_car_intact();
								close_game();
								/*
								SDL_DestroyRenderer(gRenderer);
								gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
								*/
								STATUS = Finish_Page;
								in_play = 0;
								Transition(gRenderer,0);
								finish_page.Initialize(gRenderer);
                                finish_page.show(finish_page,gRenderer,(int)car_main->is_car_intact());
							}
							//control framrate
							framerate_cap(time_start, 60);
						}
						
					}
                    else if(STATUS == InsertName_Page){
						main_menu.setPlayMusic(true);
                        if(e.key.keysym.sym == SDLK_RETURN){
                            if(inp.name.length()==0)
                                inp.name = "UNKNOWN";

                            rp.record[(int)map_parameter][(int)difficulty_parameter].setRecordName(rp.record[(int)map_parameter][(int)difficulty_parameter],inp.name);
                            

                            rp.record[(int)map_parameter][(int)difficulty_parameter].writeToFile(rp.record[(int)map_parameter][(int)difficulty_parameter],path[(int)map_parameter][(int)difficulty_parameter]);

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