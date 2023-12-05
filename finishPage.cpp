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

    const char src[3][40]={"testimage/finish.png","testimage/fail_fallen.png","testimage/fail_crush.png"};

    void Initialize(SDL_Renderer* REND){
        background.loadFromFile("testimage/black.png",REND);
        usedTime=0;finishWay=0;
    }

    void show(finishPage &f, SDL_Renderer* REND, TTF_Font* Font,int way);

    void setUsedTime(finishPage &f,double T);
};

void finishPage::setUsedTime(finishPage &f,double T){
    f.usedTime = T;
}

void finishPage::show(finishPage &f, SDL_Renderer* REND, TTF_Font* Font,int way){
    SDL_RenderClear(REND);
    f.background.loadFromFile(f.src[way],REND);
    f.background.setAlpha(64);
    f.background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
    switch(way){
        case 0:{
            std::ostringstream str;
            str << std::fixed << std::setprecision(2) << f.usedTime;
            std::string timeUsed = str.str();
	        Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",22);
            f.notification.loadFromRenderedText("FINISH!",White,REND,Font);
            f.notification.render(SCREEN_WIDTH/3,SCREEN_HEIGHT/8,SCREEN_WIDTH/3,SCREEN_HEIGHT/3,REND);
            Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",14);
            f.notification.loadFromRenderedText("You've completed",White,REND,Font);
            f.notification.render(SCREEN_WIDTH/6,SCREEN_HEIGHT/2,SCREEN_WIDTH/3*2,SCREEN_HEIGHT/4,REND);
            f.notification.loadFromRenderedText("the map in "+timeUsed+" s",White,REND,Font);
            f.notification.render(SCREEN_WIDTH/6,SCREEN_HEIGHT*0.75,SCREEN_WIDTH/3*2,SCREEN_HEIGHT/4,REND);
            SDL_RenderPresent(REND);
            break;
        }
        case 1:{
            Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",22);
            f.notification.loadFromRenderedText("FAILED!",White,REND,Font);
            f.notification.render(SCREEN_WIDTH/3,SCREEN_HEIGHT/8,SCREEN_WIDTH/3,SCREEN_HEIGHT/3,REND);
            Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",12);
            f.notification.loadFromRenderedText("You've fallen into the sea!",White,REND,Font);
            f.notification.render(SCREEN_WIDTH*0.1,SCREEN_HEIGHT/2,SCREEN_WIDTH*0.8,SCREEN_HEIGHT/4,REND);
            f.notification.loadFromRenderedText("Maybe next time ?",White,REND,Font);
            f.notification.render(SCREEN_WIDTH*0.15,SCREEN_HEIGHT*0.75,SCREEN_WIDTH*0.7,SCREEN_HEIGHT/4,REND);
            SDL_RenderPresent(REND);
        }
        case 2:{
            Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",22);
            f.notification.loadFromRenderedText("FAILED!",White,REND,Font);
            f.notification.render(SCREEN_WIDTH/3,SCREEN_HEIGHT/8,SCREEN_WIDTH/3,SCREEN_HEIGHT/3,REND);
            Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",12);
            f.notification.loadFromRenderedText("You've hit the rock!",White,REND,Font);
            f.notification.render(SCREEN_WIDTH*0.1,SCREEN_HEIGHT/2,SCREEN_WIDTH*0.8,SCREEN_HEIGHT/4,REND);
            f.notification.loadFromRenderedText("Maybe next time ?",White,REND,Font);
            f.notification.render(SCREEN_WIDTH*0.15,SCREEN_HEIGHT*0.75,SCREEN_WIDTH*0.7,SCREEN_HEIGHT/4,REND);
            SDL_RenderPresent(REND);
        }
    }
    
}

#endif