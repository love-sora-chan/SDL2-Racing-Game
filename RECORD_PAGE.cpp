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

/*const char* path[3][3]={
{"GameRecords/noon_easy.txt","GameRecords/noon_medium.txt","GameRecords/noon_hard.txt"},
{"GameRecords/dusk_easy.txt","GameRecords/dusk_medium.txt","GameRecords/dusk_hard.txt"},
{"GameRecords/night_easy.txt","GameRecords/night_medium.txt","GameRecords/night_hard.txt"}
};*/
/*
TTF_Font* Font;
TTF_Font* Font2;
*/
std::string record_mapname[3] = {"Noon","Redmoon","Night"};
std::string record_difficulty[3] = {"Easy","Medium","Hard"};

class RecordPage{
	private:
        const char* path[3][3];

        //options
        LTexture button,background;
	
	public:
		int bar_opt = 0;
        int map_option = 0;
        int diffculty_option = 0;
        Record record[3][3];

		//initialize
        RecordPage():path{
        {"GameRecords/noon_easy.txt","GameRecords/noon_medium.txt","GameRecords/noon_hard.txt"},
        {"GameRecords/dusk_easy.txt","GameRecords/dusk_medium.txt","GameRecords/dusk_hard.txt"},
        {"GameRecords/night_easy.txt","GameRecords/night_medium.txt","GameRecords/night_hard.txt"}
        }{}

		void allreset();
        void read();
		
		//show
		void show(SDL_Renderer* REND);

		//if going up or down
		void left_opt();
		void right_opt();
        void UP_DOWN_opt();

		//which page
		void mapchange(SDL_Renderer* REND);

        //get path
        const char* get_path(int i,int j){return path[i][j];}

		//appear or vanish in transition
		//void vanish(,SDL_Renderer* REND,int spd );
		//void appear(,SDL_Renderer* REND,int spd );

		
};

void RecordPage::allreset(){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            record[i][j].RecordReset(record_mapname[i],record_difficulty[j],path[i][j]);
        }
    }
}

void RecordPage::read(){
    for(int i=0;i<3;++i){
        for(int j=0;j<3;++j){
            record[i][j].loadFromFile(path[i][j]);
        }
    }
}


void RecordPage::UP_DOWN_opt(){
	bar_opt++;bar_opt = bar_opt%2;
    Mix_PlayChannel(-1,buttonSound,0);
}
void RecordPage::left_opt(){
    if(bar_opt==0){
        if(map_option==0)    map_option = 2;
        else    map_option--;
    }
    else{
        if(diffculty_option==0)  diffculty_option = 2;
        else    diffculty_option--;
    }
    Mix_PlayChannel(-1,buttonSound,0);
}
void RecordPage::right_opt(){
    if(bar_opt==0){map_option++;map_option = map_option%3;}
    else    {diffculty_option++;diffculty_option = diffculty_option%3;}
    Mix_PlayChannel(-1,buttonSound,0);
}

void RecordPage::mapchange(SDL_Renderer* REND){
    switch(map_option){
        case 0:background.loadFromFile("image/menu/noon.png",REND);break;
        case 1:background.loadFromFile("image/menu/redmoon.png",REND);break;
        case 2:background.loadFromFile("image/menu/night.png",REND);break;
    }
    background.setAlpha(64);
}

void RecordPage::show(SDL_Renderer* REND){
    SDL_RenderClear(REND);
    mapchange(REND);
    background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
    record[map_option][diffculty_option].loadFromFile(path[map_option][diffculty_option]);
    record[map_option][diffculty_option].show(REND);

    if(bar_opt==0){
        button.loadFromFile("image/menu/left_button.png",REND);
        button.render(SCREEN_WIDTH*0.25-SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*(1.0/16),SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.125,REND);
        button.loadFromFile("image/menu/right_button.png",REND);
        button.render(SCREEN_WIDTH*0.75,SCREEN_HEIGHT*(1.0/16),SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.125,REND);
    }
    else{
        button.loadFromFile("image/menu/left_button.png",REND);
        button.render(SCREEN_WIDTH*0.25-SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.25,SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.125,REND);
        button.loadFromFile("image/menu/right_button.png",REND);
        button.render(SCREEN_WIDTH*0.75,SCREEN_HEIGHT*0.25,SCREEN_HEIGHT*0.125,SCREEN_HEIGHT*0.125,REND);
    }
    SDL_RenderPresent(REND);
}




#endif