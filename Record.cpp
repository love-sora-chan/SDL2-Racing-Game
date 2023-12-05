#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>

#include "LTexture.cpp"
extern SDL_Color White;
extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;
#ifndef Record_cpp
#define Record_cpp

#define recordfile1 "recordtext.txt"


class Record{
	public:	
        int ranking[5];
        std::string MapName;
        double complete_time[5];
        std::string UserName[5];
        LTexture recordTexture;
        int new_record_ind = -1;

		//Initialize
		void RecordReset(Record &rec,std::string name);

        //write to file or load from file
        void writeToFile(Record &rec);
        void loadFromFile(Record &rec);

		//show all recorded records
		void show(Record &rec,SDL_Renderer* REND,TTF_Font* Font,TTF_Font* Font2);

		//add new record if better time
		void setRecordTime(Record &rec,double timing);

        //set player name
        void setRecordName(Record &rec,std::string name);

};

void Record::RecordReset(Record &rec,std::string name){
	rec.MapName = name;
    for(int i=0;i<5;i++){
		rec.ranking[i]=i+1;
		rec.complete_time[i]=0;
        rec.UserName[i]="UNKNOWN";
	}
    rec.writeToFile(rec);
}

void Record::loadFromFile(Record &rec){
    FILE *fp;
    fp = fopen(recordfile1,"r");
    fscanf(fp,"%s",MapName.c_str());
    MapName = (std::string)MapName.c_str();
    for(int i=0;i<5;i++){
        fscanf(fp,"%i %s %lf",&rec.ranking[i],rec.UserName[i].c_str(),&rec.complete_time[i]);
        rec.UserName[i] = (std::string)rec.UserName[i].c_str();

    }
    fclose(fp);
}

void Record::writeToFile(Record &rec){
    FILE *fp;
    fp = fopen(recordfile1,"w");
    fprintf(fp,"%s\n",rec.MapName.c_str());
    for(int i=0;i<5;i++){
        fprintf(fp,"%i\n",rec.ranking[i]);
        fprintf(fp,"%s\n",rec.UserName[i].c_str());
        fprintf(fp,"%f\n",rec.complete_time[i]);
    }
    fclose(fp);
}

void Record::show(Record &rec,SDL_Renderer* REND,TTF_Font* Font,TTF_Font* Font2){
	SDL_RenderClear(REND);
	Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",14);
    Font2 = TTF_OpenFont("test_ttf/ARCADECLASSIC.ttf",14);
	rec.recordTexture.loadFromRenderedText(rec.MapName,White,REND,Font2);
    rec.recordTexture.render(SCREEN_WIDTH*0.5-50*rec.MapName.length(),0,100*rec.MapName.length(),150,REND);
    for(int i=0;i<5;i++){
		std::ostringstream str;
        str << std::fixed << std::setprecision(2) << rec.complete_time[i];
        std::string recordTime = str.str();
		rec.recordTexture.loadFromRenderedText(recordTime+" s",White,REND,Font);
		rec.recordTexture.render(SCREEN_WIDTH/4*3,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH/4,SCREEN_HEIGHT/7,REND);
        
	}
    for(int i=0;i<5;i++){
        rec.recordTexture.loadFromRenderedText("RANK"+std::to_string(rec.ranking[i]),White,REND,Font2);
		rec.recordTexture.render(0,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH/4,SCREEN_HEIGHT/7,REND);
        rec.recordTexture.loadFromRenderedText(rec.UserName[i],White,REND,Font2);
        rec.recordTexture.render(SCREEN_WIDTH*0.3,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH*0.4*(rec.UserName[i].length()/8.0),SCREEN_HEIGHT/7,REND);
    }
	SDL_RenderPresent(REND);
}

void Record::setRecordTime(Record &rec,double timing){
    bool find = true;
    for(int i=0;find && i<5;i++){
        if(rec.complete_time[i]==0 || rec.complete_time[i]>timing){
            for(int j=4;j>i;j--){
                rec.complete_time[j]=rec.complete_time[j-1];
                rec.UserName[j]=rec.UserName[j-1];
            }
            rec.complete_time[i]=timing;
            rec.new_record_ind=i;
            find = false;
        }
    }
}

void Record::setRecordName(Record &rec,std::string name){
    if(rec.new_record_ind >= 0){
        rec.UserName[rec.new_record_ind]=name;
        rec.new_record_ind = -1;
    }
}

#endif