#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <cmath>

#include "LTexture.cpp"
extern SDL_Color White;
extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;

#ifndef Settings_cpp
#define Settings_cpp

extern double volume_m;
extern double volume_c;

extern Mix_Chunk* buttonSound;
extern Mix_Chunk* textsound;

class SettingsPage{
    private:
        LTexture background,button,text;
        int opt,LR,RQ;
        bool confirm,showEffect,setReset;
        std::string vm,vc,ql1,ql2;
        TTF_Font* settingsFont;
    
    public:
        SettingsPage(){
            settingsFont = TTF_OpenFont("font/ARCADECLASSIC.ttf",16);
        }
        ~SettingsPage(){
            TTF_CloseFont(settingsFont);
        }
        void Initialize(SDL_Renderer* REND);

        //key arrow event
		void left_opt();
		void right_opt();
        void UP_opt();
        void DOWN_opt();

        void switchPage(){confirm = !confirm;showEffect = true;setReset = false;}
        void Reset(){setReset = true;}

        void showTextEffect(SDL_Renderer* REND);
        void show(SDL_Renderer* REND);

        void volumeUp(double &v){if(v<1) v+=0.05;}
        void volumeDown(double &v){if(v>0) v-=0.05;}

        int get_opt(){return opt;}
        int get_LR(){return LR;}
        int get_RQ(){return RQ;}
        bool get_confirm(){return confirm;}
        bool get_reset(){return setReset;}
};

void SettingsPage::Initialize(SDL_Renderer* REND){
    background.loadFromFile("image/menu/main_menu.png",REND);
    opt=0;LR=0;RQ=0;
    confirm = false;showEffect = true;setReset = false;
}

void SettingsPage::showTextEffect(SDL_Renderer* REND){
    std::string temp1 = "";
    std::string temp2 = "";
    Mix_Volume(-1,64*volume_c);
    int i=0;int j=0;
    std::string line1 = "DO  YOU  WANT  TO";
    std::string line2;
    if(LR==0)   {line2 = "RESET  ALL  YOUR  RECORDS";RQ=1;}
    else    {line2 = "QUIT  THE  GAME";RQ=2;}
    while(i<line1.length()||j<line2.length()){
        SDL_RenderClear(REND);
        background.setAlpha(64);
        background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
        if(i<line1.length()){
            temp1 += line1[i];
            i++;
        }
        text.loadFromRenderedText(temp1,White,REND,settingsFont);
        text.render(SCREEN_WIDTH/2-20*temp1.length(),120,40*temp1.length(),60,REND);
        if(j<line2.length()){
            temp2 += line2[j];
            j++;
        }
        text.loadFromRenderedText(temp2,White,REND,settingsFont);
        text.render(SCREEN_WIDTH/2-20*temp2.length(),240,40*temp2.length(),60,REND);
        SDL_RenderPresent(REND);
        Mix_PlayChannel(-1,textsound,0);
        SDL_Delay(20);
    }
    ql1 = line1;
    ql2 = line2;
    SDL_Delay(100);
    Mix_Volume(-1,128*volume_c);
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);
}

void SettingsPage::show(SDL_Renderer* REND){
    SDL_RenderClear(REND);
    background.setAlpha(64);
    background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);

    if(!confirm){
        text.loadFromRenderedText("MUSIC",White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.2,SCREEN_WIDTH*0.15,SCREEN_HEIGHT*0.1,REND);
        text.loadFromRenderedText("AUDIO",White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.5,SCREEN_WIDTH*0.15,SCREEN_HEIGHT*0.1,REND);

        if(opt==0){
            button.loadFromFile("image/menu/left_button.png",REND);
            button.render(SCREEN_WIDTH*0.5,SCREEN_HEIGHT*0.2,SCREEN_HEIGHT*0.1,SCREEN_HEIGHT*0.1,REND);
            button.loadFromFile("image/menu/right_button.png",REND);
            button.render(SCREEN_WIDTH*0.85,SCREEN_HEIGHT*0.2,SCREEN_HEIGHT*0.1,SCREEN_HEIGHT*0.1,REND);
            button.loadFromFile("image/menu/reset_unchosen.png",REND);
            button.render(SCREEN_WIDTH*0.1,SCREEN_HEIGHT*0.8,SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.1,REND);
            button.loadFromFile("image/menu/quit_unchosen.png",REND);
            button.render(SCREEN_WIDTH*0.65,SCREEN_HEIGHT*0.8,SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.1,REND);
        }
        else if(opt==1){
            button.loadFromFile("image/menu/left_button.png",REND);
            button.render(SCREEN_WIDTH*0.5,SCREEN_HEIGHT*0.5,SCREEN_HEIGHT*0.1,SCREEN_HEIGHT*0.1,REND);
            button.loadFromFile("image/menu/right_button.png",REND);
            button.render(SCREEN_WIDTH*0.85,SCREEN_HEIGHT*0.5,SCREEN_HEIGHT*0.1,SCREEN_HEIGHT*0.1,REND);
            button.loadFromFile("image/menu/reset_unchosen.png",REND);
            button.render(SCREEN_WIDTH*0.1,SCREEN_HEIGHT*0.8,SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.1,REND);
            button.loadFromFile("image/menu/quit_unchosen.png",REND);
            button.render(SCREEN_WIDTH*0.65,SCREEN_HEIGHT*0.8,SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.1,REND);
        }
        else{
            if(LR==0){
                button.loadFromFile("image/menu/reset_chosen.png",REND);
                button.render(SCREEN_WIDTH*0.1,SCREEN_HEIGHT*0.8,SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.1,REND);
                button.loadFromFile("image/menu/quit_unchosen.png",REND);
                button.render(SCREEN_WIDTH*0.65,SCREEN_HEIGHT*0.8,SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.1,REND);
            }
            else{
                button.loadFromFile("image/menu/reset_unchosen.png",REND);
                button.render(SCREEN_WIDTH*0.1,SCREEN_HEIGHT*0.8,SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.1,REND);
                button.loadFromFile("image/menu/quit_chosen.png",REND);
                button.render(SCREEN_WIDTH*0.65,SCREEN_HEIGHT*0.8,SCREEN_WIDTH*0.25,SCREEN_HEIGHT*0.1,REND);
            }
        }

        vm = std::to_string((int)ceil(volume_m*100));
        vc = std::to_string((int)ceil(volume_c*100));
        text.loadFromRenderedText(vm,White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.675+SCREEN_HEIGHT*0.05-25*vm.length(),SCREEN_HEIGHT*0.2,50*vm.length(),SCREEN_HEIGHT*0.1,REND);
        text.loadFromRenderedText(vc,White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.675+SCREEN_HEIGHT*0.05-25*vc.length(),SCREEN_HEIGHT*0.5,50*vc.length(),SCREEN_HEIGHT*0.1,REND);
    }
    else if(!setReset){
        if(showEffect){showTextEffect(REND);showEffect = false;}
        text.loadFromRenderedText(ql1,White,REND,settingsFont);
        text.render(SCREEN_WIDTH/2-20*ql1.length(),120,40*ql1.length(),60,REND);
        text.loadFromRenderedText(ql2,White,REND,settingsFont);
        text.render(SCREEN_WIDTH/2-20*ql2.length(),240,40*ql2.length(),60,REND);
        text.loadFromRenderedText("YES",White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.2-60,SCREEN_HEIGHT*0.6,120,60,REND);
        text.loadFromRenderedText("NO",White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.8-40,SCREEN_HEIGHT*0.6,80,60,REND);
        button.loadFromFile("image/menu/select_rect.png",REND);
        if(LR == 0){            
            button.render(SCREEN_WIDTH*0.2-100,SCREEN_HEIGHT*0.6-20,200,100,REND);
        }
        else{
            button.render(SCREEN_WIDTH*0.8-100,SCREEN_HEIGHT*0.6-20,200,100,REND);
        }
    }
    else if(setReset){
        std::string temp = "You  have  reset";
        text.loadFromRenderedText(temp,White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.5-temp.length()*15,SCREEN_HEIGHT*0.25,temp.length()*30,45,REND);
        temp = "all  your  records";
        text.loadFromRenderedText(temp,White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.5-temp.length()*15,SCREEN_HEIGHT*0.5,temp.length()*30,45,REND);
        temp = "press  enter  to  continue";
        text.loadFromRenderedText(temp,White,REND,settingsFont);
        text.render(SCREEN_WIDTH*0.5-temp.length()*15,SCREEN_HEIGHT*0.75,temp.length()*30,45,REND);
    }
    
    SDL_RenderPresent(REND);
    
}


void SettingsPage::left_opt(){
    if(!confirm){
        if(opt==0)  volumeDown(volume_m);
        else if(opt==1) volumeDown(volume_c);
        else    LR = 0;
    }
    else{LR=0;}
    Mix_PlayChannel(-1,buttonSound,0);
}
void SettingsPage::right_opt(){
    if(!confirm){
        if(opt==0)  volumeUp(volume_m);
        else if(opt==1) volumeUp(volume_c);
        else    LR = 1;
    }
    else{LR=1;}
    Mix_PlayChannel(-1,buttonSound,0);
}
void SettingsPage::UP_opt(){if(opt>0&&!confirm)opt--;Mix_PlayChannel(-1,buttonSound,0);}
void SettingsPage::DOWN_opt(){if(opt<2&&!confirm)opt++;Mix_PlayChannel(-1,buttonSound,0);}

#endif