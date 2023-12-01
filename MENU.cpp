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
		LTexture background;

        //options
        LTexture start_button,record_button,quit_button;
	
	public:
		int opt=1;
		//initialize
		void Initialize(MENU &m,SDL_Renderer* REND);
		
		//show
		void show(MENU &m,SDL_Renderer* REND);

		//if going up or down
		void UP_opt(MENU &m);
		void DOWN_opt(MENU &m);

		//which button
		void choosing(int opt,MENU &m,SDL_Renderer* REND);

		//appear or vanish in transition
		void vanish(MENU &m,SDL_Renderer* REND,int spd );
		void appear(MENU &m,SDL_Renderer* REND,int spd );

		
};
void MENU::Initialize(MENU &m,SDL_Renderer* REND){
	m.opt = 1;m.options = 3;
	m.background.loadFromFile("testimage/main_menu.png",REND);
    m.background.setAlpha(128);
}

void MENU::show(MENU &m,SDL_Renderer* REND){
	SDL_RenderClear(REND);
	m.background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
	m.start_button.render(SCREEN_WIDTH*0.375,SCREEN_HEIGHT*0.45,SCREEN_WIDTH/4,SCREEN_HEIGHT/10,REND);
	m.record_button.render(SCREEN_WIDTH*0.375,SCREEN_HEIGHT*0.60,SCREEN_WIDTH/4,SCREEN_HEIGHT/10,REND);
    m.quit_button.render(SCREEN_WIDTH*0.375,SCREEN_HEIGHT*0.75,SCREEN_WIDTH/4,SCREEN_HEIGHT/10,REND);
	SDL_RenderPresent(REND);
}

void MENU::DOWN_opt(MENU &m){
	if(m.opt<m.options)	m.opt++;
	else m.opt = m.options;
}
void MENU::UP_opt(MENU &m){
	if(m.opt>0)	opt--;
	else m.opt = 1;
}

void MENU::choosing(int opt,MENU &m,SDL_Renderer* REND){
	switch(opt){
		case 1:{
			m.start_button.loadFromFile("testimage/start_chosen.png",REND);
			m.record_button.loadFromFile("testimage/record_unchosen.png",REND);
            m.quit_button.loadFromFile("testimage/quit_unchosen.png",REND);
			break;
		}
		case 2:{
			m.start_button.loadFromFile("testimage/start_unchosen.png",REND);
			m.record_button.loadFromFile("testimage/record_chosen.png",REND);
            m.quit_button.loadFromFile("testimage/quit_unchosen.png",REND);
			break;
		}
        case 3:{
            m.start_button.loadFromFile("testimage/start_unchosen.png",REND);
			m.record_button.loadFromFile("testimage/record_unchosen.png",REND);
            m.quit_button.loadFromFile("testimage/quit_chosen.png",REND);
            break;
        }
	}
}



void MENU::appear(MENU &m,SDL_Renderer* REND,int spd = 5){
	for(int i=0;i<256;i+=spd){
		SDL_Delay(1);
		SDL_RenderClear(REND);
		m.start_button.setAlpha(i);
		m.record_button.setAlpha(i);
		m.show(m,REND);
	}
	
}

void MENU::vanish(MENU &m,SDL_Renderer* REND,int spd = 5){
	for(int i=255;i>=0;i-=spd){
		SDL_Delay(1);
		SDL_RenderClear(REND);
		m.start_button.setAlpha(i);
		m.record_button.setAlpha(i);
		m.show(m,REND);
	}
	
}



#endif