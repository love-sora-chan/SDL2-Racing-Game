#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>

#include "LTexture.cpp"
extern SDL_Color White;
extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;

#ifndef MENU_cpp
#define MENU_cpp

class MENU{
	private:
		int options=3;
		LTexture background,text;
        bool playMusic = true;

        //options
        LTexture start_button,record_button,settings_button;
	
	public:
		int opt=1;
		//initialize
		void Initialize(SDL_Renderer* REND);
		
		//show
		void show(SDL_Renderer* REND);

		//if going up or down
		void UP_opt();
		void DOWN_opt();

		//which button
		void choosing(int opt,SDL_Renderer* REND);

		//appear or vanish in transition
		void vanish(SDL_Renderer* REND,int spd );
		void appear(SDL_Renderer* REND,int spd );

        bool PlayMusic(){return playMusic;}
        void setPlayMusic(bool play){playMusic = play;}

		
};
void MENU::Initialize(SDL_Renderer* REND){
	playMusic = true;
    opt = 1;options = 3;
	background.loadFromFile("image/menu/main_menu.png",REND);
    background.setAlpha(128);
	text.loadFromFile("image/menu/title.png",REND);
}

void MENU::show(SDL_Renderer* REND){
	SDL_RenderClear(REND);
	background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
	text.render(SCREEN_WIDTH*0.02,SCREEN_HEIGHT*0.15,SCREEN_WIDTH*0.96,SCREEN_HEIGHT*0.16,REND);
	start_button.render(SCREEN_WIDTH*0.375,SCREEN_HEIGHT*0.45,SCREEN_WIDTH/4,SCREEN_HEIGHT/10,REND);
	record_button.render(SCREEN_WIDTH*0.375,SCREEN_HEIGHT*0.60,SCREEN_WIDTH/4,SCREEN_HEIGHT/10,REND);
    settings_button.render(SCREEN_WIDTH*0.375,SCREEN_HEIGHT*0.75,SCREEN_WIDTH/4,SCREEN_HEIGHT/10,REND);
	SDL_RenderPresent(REND);
}

void MENU::DOWN_opt(){
	if(opt<options)	opt++;
	else opt = options;
}
void MENU::UP_opt(){
	if(opt>0)	opt--;
	else opt = 1;
}

void MENU::choosing(int opt,SDL_Renderer* REND){
	switch(opt){
		case 1:{
			start_button.loadFromFile("image/menu/start_chosen.png",REND);
			record_button.loadFromFile("image/menu/record_unchosen.png",REND);
            settings_button.loadFromFile("image/menu/settings_unchosen.png",REND);
			break;
		}
		case 2:{
			start_button.loadFromFile("image/menu/start_unchosen.png",REND);
			record_button.loadFromFile("image/menu/record_chosen.png",REND);
            settings_button.loadFromFile("image/menu/settings_unchosen.png",REND);
			break;
		}
        case 3:{
            start_button.loadFromFile("image/menu/start_unchosen.png",REND);
			record_button.loadFromFile("image/menu/record_unchosen.png",REND);
            settings_button.loadFromFile("image/menu/settings_chosen.png",REND);
            break;
        }
	}
}



void MENU::appear(SDL_Renderer* REND,int spd = 5){
	for(int i=0;i<256;i+=spd){
		SDL_Delay(1);
		SDL_RenderClear(REND);
		start_button.setAlpha(i);
		record_button.setAlpha(i);
        settings_button.setAlpha(i);
		show(REND);
	}
	
}

void MENU::vanish(SDL_Renderer* REND,int spd = 5){
	for(int i=255;i>=0;i-=spd){
		SDL_Delay(1);
		SDL_RenderClear(REND);
		start_button.setAlpha(i);
		record_button.setAlpha(i);
        settings_button.setAlpha(i);
		show(REND);
	}
}



#endif