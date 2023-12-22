#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "LTexture.cpp"
#include "Record.cpp"

extern SDL_Color White;
extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;

#ifndef NAME_INSERT_cpp
#define NAME_INSERT_cpp

extern Mix_Chunk* textsound;


class INP{
public:
    std::string name;
    LTexture background;
    TTF_Font* nameFont;
    LTexture nameShowing;

    INP(){
        nameFont = TTF_OpenFont("test_ttf/ARCADECLASSIC.ttf",48);
    }
    ~INP(){
        TTF_CloseFont(nameFont);
    }

    void Initialize(SDL_Renderer* REND);

    void handle_input(SDL_Renderer* REND,SDL_Event e);

    void show_centered(SDL_Renderer* REND);
};

void INP::Initialize(SDL_Renderer* REND){
    background.loadFromFile("image/menu/name_insert.png",REND);
    background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
    SDL_RenderPresent(REND);
    name = "";
}

void INP::handle_input(SDL_Renderer* REND,SDL_Event e){
    show_centered(REND);
        if(e.type==SDL_KEYDOWN){
            std::string temp = name;
            if(e.key.keysym.sym == SDLK_BACKSPACE && name.length()!=0)
                name.erase(name.length()-1);
            if(name.length()<8){                
                if((int)e.key.keysym.sym>=48&&(int)e.key.keysym.sym<=57)
                    name += (char)((int)e.key.keysym.sym);
                else if((int)e.key.keysym.sym>=97&&(int)e.key.keysym.sym<=122)
                    name += (char)((int)e.key.keysym.sym);
            }
            Mix_PlayChannel(-1,textsound,0);
        }
}

void INP::show_centered(SDL_Renderer* REND){
    SDL_RenderClear(REND);
    background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
    if(name.length()!=0){
        nameShowing.loadFromRenderedText(name,White,REND,nameFont);
        nameShowing.render(SCREEN_WIDTH/2-30*name.length(),SCREEN_HEIGHT/2-50,60*name.length(),100,REND);
    }
    SDL_RenderPresent(REND);
}

#endif