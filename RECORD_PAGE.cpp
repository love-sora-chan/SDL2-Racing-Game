#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>

#include "LTexture.cpp"
#include "Record.cpp"
extern SDL_Color White;
extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;

#ifndef RECORD_PAGE_cpp
#define RECORD_PAGE_cpp

class RecordPage{
	private:
        //record
        Record rec;

        //options
        LTexture left_button,right_button,back_button;
	
	public:
		int map=1;
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



#endif