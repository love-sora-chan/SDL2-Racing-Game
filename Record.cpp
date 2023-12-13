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


class Record{
	private:
        std::string MapName;
        std::string Difficulty;
        TTF_Font * Font;
        TTF_Font * Font2;

    
    public:	
        int ranking[5];
        double complete_time[5];
        std::string UserName[5];
        LTexture recordTexture;
        int new_record_ind = -1;

        Record(){
            Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",14);
            Font2 = TTF_OpenFont("test_ttf/ARCADECLASSIC.ttf",14);
        }
        ~Record(){
            TTF_CloseFont(Font);
            TTF_CloseFont(Font2);
        }
		//Initialize
		void RecordReset(Record &rec,std::string name,std::string difficulty,const char* path);

        //write to file or load from file
        void writeToFile(Record &rec,const char* path);
        void loadFromFile(Record &rec,const char* path);

		//show all recorded records
		void show(Record &rec,SDL_Renderer* REND);

		//add new record if better time
		void setRecordTime(Record &rec,double timing);

        //set player name
        void setRecordName(Record &rec,std::string name);

};

void Record::RecordReset(Record &rec,std::string name,std::string difficulty,const char* path){
	rec.MapName = name;
    rec.Difficulty = difficulty;
    for(int i=0;i<5;i++){
		rec.ranking[i]=i+1;
		rec.complete_time[i]=0;
        rec.UserName[i]="UNKNOWN";
	}
    rec.writeToFile(rec,path);
}

void Record::loadFromFile(Record &rec,const char* path){
    FILE *fp;
    fp = fopen(path,"r");
    fscanf(fp,"%s",MapName.c_str());
    MapName = (std::string)MapName.c_str();
    fscanf(fp,"%s",Difficulty.c_str());
    Difficulty = (std::string)Difficulty.c_str();
    for(int i=0;i<5;i++){
        fscanf(fp,"%i %s %lf",&rec.ranking[i],rec.UserName[i].c_str(),&rec.complete_time[i]);
        rec.UserName[i] = (std::string)rec.UserName[i].c_str();

    }
    fclose(fp);
}

void Record::writeToFile(Record &rec,const char* path){
    FILE *fp;
    fp = fopen(path,"w");
    fprintf(fp,"%s\n",rec.MapName.c_str());
    fprintf(fp,"%s\n",rec.Difficulty.c_str());
    for(int i=0;i<5;i++){
        fprintf(fp,"%i\n",rec.ranking[i]);
        fprintf(fp,"%s\n",rec.UserName[i].c_str());
        fprintf(fp,"%f\n",rec.complete_time[i]);
    }
    fclose(fp);
}

void Record::show(Record &rec,SDL_Renderer* REND){
	rec.recordTexture.loadFromRenderedText(rec.MapName,White,REND,Font2);
    rec.recordTexture.render(SCREEN_WIDTH*0.5-30*rec.MapName.length(),0,60*rec.MapName.length(),SCREEN_HEIGHT*0.25,REND);
    rec.recordTexture.loadFromRenderedText(rec.Difficulty,White,REND,Font2);
    rec.recordTexture.render(SCREEN_WIDTH*0.5-30*rec.Difficulty.length(),SCREEN_HEIGHT*0.25,60*rec.Difficulty.length(),SCREEN_HEIGHT*0.125,REND);
    for(int i=0;i<5;i++){
		std::ostringstream str;
        str << std::fixed << std::setprecision(2) << rec.complete_time[i];
        std::string recordTime = str.str();
		rec.recordTexture.loadFromRenderedText(recordTime+" s",White,REND,Font);
		rec.recordTexture.render(SCREEN_WIDTH/4*3,SCREEN_HEIGHT/8*(i+3),SCREEN_WIDTH/4,SCREEN_HEIGHT/8,REND);
        
	}
    for(int i=0;i<5;i++){
        rec.recordTexture.loadFromRenderedText("RANK"+std::to_string(rec.ranking[i]),White,REND,Font2);
		rec.recordTexture.render(0,SCREEN_HEIGHT/8*(i+3),SCREEN_WIDTH/4,SCREEN_HEIGHT/8,REND);
        rec.recordTexture.loadFromRenderedText(rec.UserName[i],White,REND,Font2);
        rec.recordTexture.render(SCREEN_WIDTH*0.3,SCREEN_HEIGHT/8.0*(i+3),SCREEN_WIDTH*0.4*(rec.UserName[i].length()/8.0),SCREEN_HEIGHT/8.0,REND);
    }
}

void Record::setRecordTime(Record &rec,double timing){
    bool find = true;
    for(int i=0;find && i<5;++i){
        if(rec.complete_time[i]==0 || rec.complete_time[i]>timing){
            for(int j=4;j>i;--j){
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