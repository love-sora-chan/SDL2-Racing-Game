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
#ifndef Record_cpp
#define Record_cpp


class Record{
	private:
		int ranking[5];
		LTexture ranking_list;
		
	public:
        float complete_time[5];
        LTexture time_list;
        
        std::string UserName[5];
        LTexture name_list;

		//Initialize
		void RecordReset(Record &rec);

		//show all recorded records
		void show(Record &rec,SDL_Renderer* REND,TTF_Font* Font,TTF_Font* Font2);


		
		
		//add new record if better time
		void setRecord(Record &rec,float timing,std::string name);

};

void Record::RecordReset(Record &rec){
	for(int i=0;i<5;i++){
		rec.ranking[i]=i+1;
		rec.complete_time[i]=0;
        rec.UserName[i]="UNKNOWN";
	}
}

void Record::show(Record &rec,SDL_Renderer* REND,TTF_Font* Font,TTF_Font* Font2){
	SDL_RenderClear(REND);
	Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",14);
	for(int i=0;i<5;i++){
		std::ostringstream str;
        str << std::fixed << std::setprecision(2) << rec.complete_time[i];
        std::string recordTime = str.str();
		rec.time_list.loadFromRenderedText(recordTime+" s",White,REND,Font);
		rec.time_list.render(SCREEN_WIDTH/4*3,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH/4,SCREEN_HEIGHT/7,REND);
        
	}
    Font2 = TTF_OpenFont("test_ttf/ARCADECLASSIC.ttf",14);
    for(int i=0;i<5;i++){
        rec.ranking_list.loadFromRenderedText("RANK"+std::to_string(rec.ranking[i]),White,REND,Font2);
		rec.ranking_list.render(0,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH/4,SCREEN_HEIGHT/7,REND);
        rec.name_list.loadFromRenderedText(rec.UserName[i],White,REND,Font2);
        rec.name_list.render(SCREEN_WIDTH*0.3,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH*0.4*(rec.UserName[i].length()/8.0),SCREEN_HEIGHT/7,REND);
    }
	SDL_RenderPresent(REND);
}

void Record::setRecord(Record &rec,float timing,std::string name){
    bool find = true;
    for(int i=0;find && i<5;i++){
        if(rec.complete_time[i]==0 || rec.complete_time[i]>timing){
            for(int j=4;j>i;j--){
                rec.complete_time[j]=rec.complete_time[j-1];
                rec.UserName[j]=rec.UserName[j-1];
            }
            rec.complete_time[i]=timing;
            rec.UserName[i] = name;
            find = false;
        }
    }
}

#endif