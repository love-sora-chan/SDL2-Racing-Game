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
extern Mix_Chunk* buttonSound;

#ifndef RECORD_PAGE_cpp
#define RECORD_PAGE_cpp

const char* path[3][3]={
{"GameRecords/noon_easy.txt","GameRecords/noon_medium.txt","GameRecords/noon_hard.txt"},
{"GameRecords/dusk_easy.txt","GameRecords/dusk_medium.txt","GameRecords/dusk_hard.txt"},
{"GameRecords/night_easy.txt","GameRecords/night_medium.txt","GameRecords/night_hard.txt"}
};
TTF_Font* Font;
TTF_Font* Font2;
std::string record_mapname[3] = {"Noon","Dusk","Night"};
std::string record_difficulty[3] = {"Easy","Medium","Hard"};

class RecordPage{
	private:

        //options
        LTexture button,background;
	
	public:
		int bar_opt = 0;
        int map_option = 0;
        int diffculty_option = 0;

        Record record[3][3];
		//initialize
		void allreset(RecordPage &rp);
        void read(RecordPage &rp);
		
		//show
		void show(RecordPage &rp,SDL_Renderer* REND);

		//if going up or down
		void left_opt(RecordPage &rp);
		void right_opt(RecordPage &rp);
        void UP_DOWN_opt(RecordPage &rp);

		//which page
		void mapchange(RecordPage &rp,SDL_Renderer* REND);

		//appear or vanish in transition
		//void vanish(RecordPage &rp,SDL_Renderer* REND,int spd );
		//void appear(RecordPage &rp,SDL_Renderer* REND,int spd );

		
};

void RecordPage::allreset(RecordPage &rp){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            rp.record[i][j].RecordReset(rp.record[i][j],record_mapname[i],record_difficulty[j],path[i][j]);
        }
    }
}

void RecordPage::read(RecordPage &rp){
    for(int i=0;i<3;++i){
        for(int j=0;j<3;++j){
            rp.record[i][j].loadFromFile(rp.record[i][j],path[i][j]);
        }
    }
}


void RecordPage::UP_DOWN_opt(RecordPage &rp){
	rp.bar_opt++;rp.bar_opt = rp.bar_opt%2;
    Mix_PlayChannel(-1,buttonSound,0);
}
void RecordPage::left_opt(RecordPage &rp){
    if(rp.bar_opt==0){
        if(rp.map_option==0)    rp.map_option = 2;
        else    rp.map_option--;
    }
    else{
        if(rp.diffculty_option==0)  rp.diffculty_option = 2;
        else    diffculty_option--;
    }
    Mix_PlayChannel(-1,buttonSound,0);
}
void RecordPage::right_opt(RecordPage &rp){
    if(rp.bar_opt==0){rp.map_option++;rp.map_option = rp.map_option%3;}
    else    {rp.diffculty_option++;rp.diffculty_option = rp.diffculty_option%3;}
    Mix_PlayChannel(-1,buttonSound,0);
}

void RecordPage::mapchange(RecordPage &rp,SDL_Renderer* REND){
    switch(rp.map_option){
        case 0:rp.background.loadFromFile("image/menu/noon.png",REND);break;
        case 1:rp.background.loadFromFile("image/menu/dusk.png",REND);break;
        case 2:rp.background.loadFromFile("image/menu/night.png",REND);break;
    }
    rp.background.setAlpha(64);
}

void RecordPage::show(RecordPage &rp,SDL_Renderer* REND){
    SDL_RenderClear(REND);
    rp.mapchange(rp,REND);
    rp.background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
    rp.record[rp.map_option][rp.diffculty_option].loadFromFile(rp.record[rp.map_option][rp.diffculty_option],path[rp.map_option][rp.diffculty_option]);
    rp.record[rp.map_option][rp.diffculty_option].show(rp.record[rp.map_option][rp.diffculty_option],REND);

    if(rp.bar_opt==0){
        rp.button.loadFromFile("image/menu/left_button.png",REND);
        rp.button.render(SCREEN_WIDTH*0.25-SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*(1.0/16),SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.125,REND);
        rp.button.loadFromFile("image/menu/right_button.png",REND);
        rp.button.render(SCREEN_WIDTH*0.75,SCREEN_HEIGHT*(1.0/16),SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.125,REND);
    }
    else{
        rp.button.loadFromFile("image/menu/left_button.png",REND);
        rp.button.render(SCREEN_WIDTH*0.25-SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.25,SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.125,REND);
        rp.button.loadFromFile("image/menu/right_button.png",REND);
        rp.button.render(SCREEN_WIDTH*0.75,SCREEN_HEIGHT*0.25,SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.125,REND);
    }
    SDL_RenderPresent(REND);
}




#endif