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
		float complete_time[5];
		int ranking[5];
		LTexture ranking_list[5];
		LTexture time_list[5];

	public:
		//Initialize
		void RecordReset(Record &rec);

		//show all recorded records
		void show(Record &rec,SDL_Renderer* REND,TTF_Font* Font);

		
		
		//add new record if better time
		//void setRecord(Record &rec,float timing);

};

void Record::RecordReset(Record &rec){
	for(int i=0;i<5;i++){
		rec.ranking[i]=i+1;
		rec.complete_time[i]=0;
		rec.time_list[i].free();
		rec.ranking_list[i].free();
	}
}

void Record::show(Record &rec,SDL_Renderer* REND,TTF_Font* Font){
	SDL_RenderClear(REND);
	Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",14);
	for(int i=0;i<5;i++){
		std::ostringstream str;
        str << std::fixed << std::setprecision(2) << rec.complete_time[i];
        std::string recordTime = str.str();
        rec.ranking_list->loadFromRenderedText("RANK"+std::to_string(rec.ranking[i]),White,REND,Font);
		rec.ranking_list->render(0,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH/4,SCREEN_HEIGHT/7,REND);
		rec.time_list->loadFromRenderedText(recordTime+" s",White,REND,Font);
		rec.time_list->render(SCREEN_WIDTH/4*3,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH/4,SCREEN_HEIGHT/7,REND);
	}
	SDL_RenderPresent(REND);
}

#endif