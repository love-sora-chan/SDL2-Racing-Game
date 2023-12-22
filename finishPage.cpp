#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "LTexture.cpp"
extern SDL_Color White;
extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;

#ifndef FINISH_cpp
#define FINISH_cpp

class finishPage{
public:
    double usedTime;
    int finishWay;
    LTexture background,notification;
    TTF_Font* Font;

    const char src[3][40]={"image/menu/finish.png","image/menu/fail_fallen.png","image/menu/fail_crush.png"};

    finishPage(){
        Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",22);
    }
    ~finishPage(){
        TTF_CloseFont(Font);
    }
    void Initialize(SDL_Renderer* REND){
        background.loadFromFile("image/menu/black.png",REND);
    }

    void show(SDL_Renderer* REND,int way);

    void setUsedTime(double T);
};

void finishPage::setUsedTime(double T){
    usedTime = T;
}

void finishPage::show(SDL_Renderer* REND,int way){
    SDL_RenderClear(REND);
    background.loadFromFile(src[way],REND);
    background.setAlpha(64);
    background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
    switch(way){
        case 0:{
            std::ostringstream str;
            str << std::fixed << std::setprecision(2) << usedTime;
            std::string timeUsed = str.str();
	        TTF_SetFontSize(Font,22);
            notification.loadFromRenderedText("FINISH!",White,REND,Font);
            notification.render(SCREEN_WIDTH/3,SCREEN_HEIGHT/8,SCREEN_WIDTH/3,SCREEN_HEIGHT/3,REND);
            TTF_SetFontSize(Font,14);
            notification.loadFromRenderedText("You've completed",White,REND,Font);
            notification.render(SCREEN_WIDTH/6,SCREEN_HEIGHT/2,SCREEN_WIDTH/3*2,SCREEN_HEIGHT/4,REND);
            notification.loadFromRenderedText("the map in "+timeUsed+" s",White,REND,Font);
            notification.render(SCREEN_WIDTH/6,SCREEN_HEIGHT*0.75,SCREEN_WIDTH/3*2,SCREEN_HEIGHT/4,REND);
            SDL_RenderPresent(REND);
            break;
        }
        case 1:{
            TTF_SetFontSize(Font,22);
            notification.loadFromRenderedText("FAILED!",White,REND,Font);
            notification.render(SCREEN_WIDTH/3,SCREEN_HEIGHT/8,SCREEN_WIDTH/3,SCREEN_HEIGHT/3,REND);
            TTF_SetFontSize(Font,12);
            notification.loadFromRenderedText("You've fallen into the sea!",White,REND,Font);
            notification.render(SCREEN_WIDTH*0.1,SCREEN_HEIGHT/2,SCREEN_WIDTH*0.8,SCREEN_HEIGHT/4,REND);
            notification.loadFromRenderedText("Maybe next time ?",White,REND,Font);
            notification.render(SCREEN_WIDTH*0.15,SCREEN_HEIGHT*0.75,SCREEN_WIDTH*0.7,SCREEN_HEIGHT/4,REND);
            SDL_RenderPresent(REND);
            break;
        }
        case 2:{
            TTF_SetFontSize(Font,22);
            notification.loadFromRenderedText("FAILED!",White,REND,Font);
            notification.render(SCREEN_WIDTH/3,SCREEN_HEIGHT/8,SCREEN_WIDTH/3,SCREEN_HEIGHT/3,REND);
            TTF_SetFontSize(Font,12);
            notification.loadFromRenderedText("You've hit the rock!",White,REND,Font);
            notification.render(SCREEN_WIDTH*0.1,SCREEN_HEIGHT/2,SCREEN_WIDTH*0.8,SCREEN_HEIGHT/4,REND);
            notification.loadFromRenderedText("Maybe next time ?",White,REND,Font);
            notification.render(SCREEN_WIDTH*0.15,SCREEN_HEIGHT*0.75,SCREEN_WIDTH*0.7,SCREEN_HEIGHT/4,REND);
            SDL_RenderPresent(REND);
            break;
        }
    }
    
}

#endif