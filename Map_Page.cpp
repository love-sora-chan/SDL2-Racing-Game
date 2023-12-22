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
        MapPage(){
            mapFont = TTF_OpenFont("font/ARCADECLASSIC.ttf",16);
        }
        ~MapPage(){
            TTF_CloseFont(mapFont);
        }
        void Initialize(){
            map_opt=0;car_opt=0;difficulty_opt=0;
            mapChosen=false;onDiff=false;showEffect=true;
            mapname = "";
        }
        



    	void show(SDL_Renderer* REND);

		//key arrow event
		void left_opt();
		void right_opt();
        void UP_opt();
        void DOWN_opt();

        void showNameEffect(std::string str,SDL_Renderer* REND);

        
        void switchPage(){mapChosen = !mapChosen;showEffect = true;}

        int getMap(){return map_opt;}
        int getCar(){return car_opt;}
        int getDifficulty(){return difficulty_opt;}
        bool ifMapChosen(){return mapChosen;}
        bool CarOrDifficulty(){return onDiff;}
        
};

void MapPage::left_opt(){
    if(!ifMapChosen()){
        map_opt+=2;
        map_opt %= 3;
    }
    else{
        if(!CarOrDifficulty()){
            car_opt++;car_opt%=2;
        }
        else{
            difficulty_opt+=2;difficulty_opt%=3;
        }
    }
    Mix_PlayChannel(-1,buttonSound,0);
}
void MapPage::right_opt(){
    if(!ifMapChosen()){
        map_opt++;
        map_opt %= 3;
    }
    else{
        if(!CarOrDifficulty()){
            car_opt++;car_opt%=2;
        }
        else{
            difficulty_opt++;difficulty_opt%=3;
        }
    }
    Mix_PlayChannel(-1,buttonSound,0);
}
void MapPage::UP_opt(){
    if(ifMapChosen()){
        if(CarOrDifficulty()){onDiff = false;}
    }
    Mix_PlayChannel(-1,buttonSound,0);
}
void MapPage::DOWN_opt(){
    if(ifMapChosen()){
        if(!CarOrDifficulty()){onDiff = true;}
    }
    Mix_PlayChannel(-1,buttonSound,0);
}

void MapPage::showNameEffect(std::string str,SDL_Renderer* REND){
    std::string temp = "";
    Mix_Volume(-1,64);
    for(int i=0;i<str.length();i++){
        SDL_RenderClear(REND);
        map.render(40,40,560,560,REND);
        temp += str[i];
        text.loadFromRenderedText(temp,White,REND,mapFont);
        text.render(320-20*temp.length(),630,40*temp.length(),60,REND);
        SDL_RenderPresent(REND);
        Mix_PlayChannel(-1,textsound,0);
        SDL_Delay(20);
    }
    mapname = str;
    SDL_Delay(100);
    Mix_Volume(-1,128);
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);
}

void MapPage::show(SDL_Renderer* REND){
    SDL_RenderClear(REND);
    //background
    background.loadFromFile("image/menu/black.png",REND);
    background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
    if(!ifMapChosen()){
    
        //map picture
        switch(getMap()){
            case 0:{
                map2.setAlpha(16);
                map2.loadFromFile("image/menu/night.png",REND);
                map2.render(SCREEN_WIDTH*0.5-480,SCREEN_HEIGHT-480,320,320,REND);
                map2.loadFromFile("image/menu/dusk.png",REND);
                map2.render(SCREEN_WIDTH*0.5+160,SCREEN_HEIGHT-480,320,320,REND);

                map.setAlpha(255);
                map.loadFromFile("image/menu/noon.png",REND);
                map.render(SCREEN_WIDTH*0.5-240,SCREEN_HEIGHT-560,480,480,REND);

                break;
            }
            case 1:{
                map2.setAlpha(16);
                map2.loadFromFile("image/menu/noon.png",REND);
                map2.render(SCREEN_WIDTH*0.5-480,SCREEN_HEIGHT-480,320,320,REND);
                map2.loadFromFile("image/menu/night.png",REND);
                map2.render(SCREEN_WIDTH*0.5+160,SCREEN_HEIGHT-480,320,320,REND);

                map.setAlpha(255);
                map.loadFromFile("image/menu/dusk.png",REND);
                map.render(SCREEN_WIDTH*0.5-240,SCREEN_HEIGHT-560,480,480,REND);

                break;
            }
            case 2:{
                map2.setAlpha(16);
                map2.loadFromFile("image/menu/dusk.png",REND);
                map2.render(SCREEN_WIDTH*0.5-480,SCREEN_HEIGHT-480,320,320,REND);
                map2.loadFromFile("image/menu/noon.png",REND);
                map2.render(SCREEN_WIDTH*0.5+160,SCREEN_HEIGHT-480,320,320,REND);

                map.setAlpha(255);
                map.loadFromFile("image/menu/night.png",REND);
                map.render(SCREEN_WIDTH*0.5-240,SCREEN_HEIGHT-560,480,480,REND);

                break;
            }
        }
        //text
        text.loadFromRenderedText("Select the map",White,REND,mapFont);
        text.render(SCREEN_WIDTH*0.2,0,SCREEN_WIDTH*0.6,100,REND);
        //button
        button.loadFromFile("image/menu/left_button.png",REND);
        button.render(20,350,100,100,REND);
        button.loadFromFile("image/menu/right_button.png",REND);
        button.render(SCREEN_WIDTH-120,350,100,100,REND);
    }
    else{
        switch(getMap()){
            case 0:{
                map.loadFromFile("image/menu/noon.png",REND);
                if(showEffect){showNameEffect("SEAWAY NOON",REND);showEffect = false;}
                else{
                    map.render(40,40,560,560,REND);
                    text.loadFromRenderedText(mapname,White,REND,mapFont);
                    text.render(320-20*mapname.length(),630,40*mapname.length(),60,REND);
                }
                break;
            }
            case 1:{
                map.loadFromFile("image/menu/dusk.png",REND);
                if(showEffect){showNameEffect("SEAWAY DUSK",REND);showEffect = false;}
                else{
                    map.render(40,40,560,560,REND);
                    text.loadFromRenderedText(mapname,White,REND,mapFont);
                    text.render(320-20*mapname.length(),630,40*mapname.length(),60,REND);
                }
                break;
            }
            case 2:{
                map.loadFromFile("image/menu/night.png",REND);
                if(showEffect){showNameEffect("SEAWAY NIGHT",REND);showEffect = false;}
                else{
                    map.render(40,40,560,560,REND);
                    text.loadFromRenderedText(mapname,White,REND,mapFont);
                    text.render(320-20*mapname.length(),630,40*mapname.length(),60,REND);
                }
                break;
            }
        }
        text.loadFromRenderedText("car",White,REND,mapFont);
        text.render(SCREEN_WIDTH-380,60,120,60,REND);
        std::string car;
        if(getCar()==0){car = "AE86";}
        else{car = "R7X";}
        text.loadFromRenderedText(car,White,REND,mapFont);
        text.render(SCREEN_WIDTH-320-(car.length()*20),180,(car.length()*40),60,REND);
        text.loadFromRenderedText("difficulty",White,REND,mapFont);
        text.render(SCREEN_WIDTH-520,420,400,60,REND);
        std::string diff;
        if(getDifficulty()==0){diff = "easy";}
        else if(getDifficulty()==1){diff = "medium";}
        else{diff = "hard";}
        text.loadFromRenderedText(diff,White,REND,mapFont);
        text.render(SCREEN_WIDTH-320-(diff.length()*20),540,(diff.length()*40),60,REND);

        if(!CarOrDifficulty()){
            button.loadFromFile("image/menu/left_button.png",REND);
            button.render(680,180,60,60,REND);
            button.loadFromFile("image/menu/right_button.png",REND);
            button.render(1180,180,60,60,REND);
        }
        else{
            button.loadFromFile("image/menu/left_button.png",REND);
            button.render(680,540,60,60,REND);
            button.loadFromFile("image/menu/right_button.png",REND);
            button.render(1180,540,60,60,REND);
        }
    }
    SDL_RenderPresent(REND);
}

#endif