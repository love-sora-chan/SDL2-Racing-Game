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

#ifndef MAP_PAGE
#define MAP_PAGE
extern Mix_Chunk* textsound;
extern Mix_Chunk* buttonSound;
class MapPage{
    private:
        int map_opt,car_opt,difficulty_opt;
        std::string mapname;
        LTexture background;
        LTexture map,map2;
        LTexture text;
        LTexture button;
        TTF_Font* mapFont;
        bool mapChosen, onDiff, showEffect;
    public:
        void Initialize(){
            map_opt=0;car_opt=0;difficulty_opt=0;
            mapChosen=false;onDiff=false;showEffect=true;
            mapname = "";
        }
        



    	void show(MapPage &mp,SDL_Renderer* REND);

		//key arrow event
		void left_opt(MapPage &mp);
		void right_opt(MapPage &mp);
        void UP_opt(MapPage &mp);
        void DOWN_opt(MapPage &mp);

        void showNameEffect(MapPage &mp,std::string str,SDL_Renderer* REND);

        
        void switchPage(){mapChosen = !mapChosen;showEffect = true;}

        int getMap(){return map_opt;}
        int getCar(){return car_opt;}
        int getDifficulty(){return difficulty_opt;}
        bool ifMapChosen(){return mapChosen;}
        bool CarOrDifficulty(){return onDiff;}
        
};

void MapPage::left_opt(MapPage &mp){
    if(!mp.ifMapChosen()){
        mp.map_opt+=2;
        mp.map_opt %= 3;
    }
    else{
        if(!mp.CarOrDifficulty()){
            mp.car_opt++;mp.car_opt%=2;
        }
        else{
            mp.difficulty_opt+=2;mp.difficulty_opt%=3;
        }
    }
    Mix_PlayChannel(-1,buttonSound,0);
}
void MapPage::right_opt(MapPage &mp){
    if(!mp.ifMapChosen()){
        mp.map_opt++;
        mp.map_opt %= 3;
    }
    else{
        if(!mp.CarOrDifficulty()){
            mp.car_opt++;mp.car_opt%=2;
        }
        else{
            mp.difficulty_opt++;mp.difficulty_opt%=3;
        }
    }
    Mix_PlayChannel(-1,buttonSound,0);
}
void MapPage::UP_opt(MapPage &mp){
    if(mp.ifMapChosen()){
        if(mp.CarOrDifficulty()){mp.onDiff = false;}
    }
    Mix_PlayChannel(-1,buttonSound,0);
}
void MapPage::DOWN_opt(MapPage &mp){
    if(mp.ifMapChosen()){
        if(!mp.CarOrDifficulty()){mp.onDiff = true;}
    }
    Mix_PlayChannel(-1,buttonSound,0);
}

void MapPage::showNameEffect(MapPage &mp,std::string str,SDL_Renderer* REND){
    std::string temp = "";
    Mix_Volume(-1,64);
    for(int i=0;i<str.length();i++){
        SDL_RenderClear(REND);
        mp.map.render(40,40,560,560,REND);
        temp += str[i];
        mp.text.loadFromRenderedText(temp,White,REND,mapFont);
        mp.text.render(320-20*temp.length(),630,40*temp.length(),60,REND);
        SDL_RenderPresent(REND);
        Mix_PlayChannel(-1,textsound,0);
        SDL_Delay(20);
    }
    mp.mapname = str;
    SDL_Delay(100);
    Mix_Volume(-1,128);
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);
}

void MapPage::show(MapPage &mp,SDL_Renderer* REND){
    SDL_RenderClear(REND);
    mp.mapFont = TTF_OpenFont("test_ttf/ARCADECLASSIC.ttf",16);
    //background
    mp.background.loadFromFile("image/menu/black.png",REND);
    mp.background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
    if(!mp.ifMapChosen()){
    
        //map picture
        switch(mp.getMap()){
            case 0:{
                mp.map2.setAlpha(16);
                mp.map2.loadFromFile("image/menu/night.png",REND);
                mp.map2.render(SCREEN_WIDTH*0.5-480,SCREEN_HEIGHT-480,320,320,REND);
                mp.map2.loadFromFile("image/menu/dusk.png",REND);
                mp.map2.render(SCREEN_WIDTH*0.5+160,SCREEN_HEIGHT-480,320,320,REND);

                mp.map.setAlpha(255);
                mp.map.loadFromFile("image/menu/noon.png",REND);
                mp.map.render(SCREEN_WIDTH*0.5-240,SCREEN_HEIGHT-560,480,480,REND);

                break;
            }
            case 1:{
                mp.map2.setAlpha(16);
                mp.map2.loadFromFile("image/menu/noon.png",REND);
                mp.map2.render(SCREEN_WIDTH*0.5-480,SCREEN_HEIGHT-480,320,320,REND);
                mp.map2.loadFromFile("image/menu/night.png",REND);
                mp.map2.render(SCREEN_WIDTH*0.5+160,SCREEN_HEIGHT-480,320,320,REND);

                mp.map.setAlpha(255);
                mp.map.loadFromFile("image/menu/dusk.png",REND);
                mp.map.render(SCREEN_WIDTH*0.5-240,SCREEN_HEIGHT-560,480,480,REND);

                break;
            }
            case 2:{
                mp.map2.setAlpha(16);
                mp.map2.loadFromFile("image/menu/dusk.png",REND);
                mp.map2.render(SCREEN_WIDTH*0.5-480,SCREEN_HEIGHT-480,320,320,REND);
                mp.map2.loadFromFile("image/menu/noon.png",REND);
                mp.map2.render(SCREEN_WIDTH*0.5+160,SCREEN_HEIGHT-480,320,320,REND);

                mp.map.setAlpha(255);
                mp.map.loadFromFile("image/menu/night.png",REND);
                mp.map.render(SCREEN_WIDTH*0.5-240,SCREEN_HEIGHT-560,480,480,REND);

                break;
            }
        }
        //text
        mp.text.loadFromRenderedText("Select the map",White,REND,mp.mapFont);
        mp.text.render(SCREEN_WIDTH*0.2,0,SCREEN_WIDTH*0.6,100,REND);
        //button
        mp.button.loadFromFile("image/menu/left_button.png",REND);
        mp.button.render(20,350,100,100,REND);
        mp.button.loadFromFile("image/menu/right_button.png",REND);
        mp.button.render(SCREEN_WIDTH-120,350,100,100,REND);
    }
    else{
        switch(mp.getMap()){
            case 0:{
                mp.map.loadFromFile("image/menu/noon.png",REND);
                if(mp.showEffect){mp.showNameEffect(mp,"SEAWAY NOON",REND);mp.showEffect = false;}
                else{
                    mp.map.render(40,40,560,560,REND);
                    mp.text.loadFromRenderedText(mp.mapname,White,REND,mapFont);
                    mp.text.render(320-20*mp.mapname.length(),630,40*mp.mapname.length(),60,REND);
                }
                break;
            }
            case 1:{
                mp.map.loadFromFile("image/menu/dusk.png",REND);
                if(mp.showEffect){mp.showNameEffect(mp,"SEAWAY DUSK",REND);mp.showEffect = false;}
                else{
                    mp.map.render(40,40,560,560,REND);
                    mp.text.loadFromRenderedText(mp.mapname,White,REND,mapFont);
                    mp.text.render(320-20*mp.mapname.length(),630,40*mp.mapname.length(),60,REND);
                }
                break;
            }
            case 2:{
                mp.map.loadFromFile("image/menu/night.png",REND);
                if(mp.showEffect){mp.showNameEffect(mp,"SEAWAY NIGHT",REND);mp.showEffect = false;}
                else{
                    mp.map.render(40,40,560,560,REND);
                    mp.text.loadFromRenderedText(mp.mapname,White,REND,mapFont);
                    mp.text.render(320-20*mp.mapname.length(),630,40*mp.mapname.length(),60,REND);
                }
                break;
            }
        }
        mp.text.loadFromRenderedText("car",White,REND,mp.mapFont);
        mp.text.render(SCREEN_WIDTH-380,60,120,60,REND);
        std::string car;
        if(mp.getCar()==0){car = "AE86";}
        else{car = "R7X";}
        mp.text.loadFromRenderedText(car,White,REND,mp.mapFont);
        mp.text.render(SCREEN_WIDTH-320-(car.length()*20),180,(car.length()*40),60,REND);
        mp.text.loadFromRenderedText("difficulty",White,REND,mp.mapFont);
        mp.text.render(SCREEN_WIDTH-520,420,400,60,REND);
        std::string diff;
        if(mp.getDifficulty()==0){diff = "easy";}
        else if(mp.getDifficulty()==1){diff = "medium";}
        else{diff = "hard";}
        mp.text.loadFromRenderedText(diff,White,REND,mp.mapFont);
        mp.text.render(SCREEN_WIDTH-320-(diff.length()*20),540,(diff.length()*40),60,REND);

        if(!mp.CarOrDifficulty()){
            mp.button.loadFromFile("image/menu/left_button.png",REND);
            mp.button.render(680,180,60,60,REND);
            mp.button.loadFromFile("image/menu/right_button.png",REND);
            mp.button.render(1180,180,60,60,REND);
        }
        else{
            mp.button.loadFromFile("image/menu/left_button.png",REND);
            mp.button.render(680,540,60,60,REND);
            mp.button.loadFromFile("image/menu/right_button.png",REND);
            mp.button.render(1180,540,60,60,REND);
        }
    }
    SDL_RenderPresent(REND);
}

#endif