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
		void RecordReset(std::string name,std::string difficulty,const char* path);

        //write to file or load from file
        void writeToFile(const char* path);
        void loadFromFile(const char* path);

		//show all recorded records
		void show(SDL_Renderer* REND);

		//add new record if better time
		void setRecordTime(double timing);

        //set player name
        void setRecordName(std::string name);

};

void Record::RecordReset(std::string name,std::string difficulty,const char* path){
	MapName = name;
    Difficulty = difficulty;
    for(int i=0;i<5;i++){
		ranking[i]=i+1;
		complete_time[i]=0;
        UserName[i]="UNKNOWN";
	}
    writeToFile(path);
}

void Record::loadFromFile(const char* path){
    FILE *fp;
    fp = fopen(path,"r");
    fscanf(fp,"%s",MapName.c_str());
    MapName = (std::string)MapName.c_str();
    fscanf(fp,"%s",Difficulty.c_str());
    Difficulty = (std::string)Difficulty.c_str();
    for(int i=0;i<5;i++){
        fscanf(fp,"%i %s %lf",&ranking[i],UserName[i].c_str(),&complete_time[i]);
        UserName[i] = (std::string)UserName[i].c_str();

    }
    fclose(fp);
}

void Record::writeToFile(const char* path){
    FILE *fp;
    fp = fopen(path,"w");
    fprintf(fp,"%s\n",MapName.c_str());
    fprintf(fp,"%s\n",Difficulty.c_str());
    for(int i=0;i<5;i++){
        fprintf(fp,"%i\n",ranking[i]);
        fprintf(fp,"%s\n",UserName[i].c_str());
        fprintf(fp,"%f\n",complete_time[i]);
    }
    fclose(fp);
}

void Record::show(SDL_Renderer* REND){
	recordTexture.loadFromRenderedText(MapName,White,REND,Font2);
    recordTexture.render(SCREEN_WIDTH*0.5-30*MapName.length(),0,60*MapName.length(),SCREEN_HEIGHT*0.25,REND);
    recordTexture.loadFromRenderedText(Difficulty,White,REND,Font2);
    recordTexture.render(SCREEN_WIDTH*0.5-30*Difficulty.length(),SCREEN_HEIGHT*0.25,60*Difficulty.length(),SCREEN_HEIGHT*0.125,REND);
    for(int i=0;i<5;i++){
		std::ostringstream str;
        str << std::fixed << std::setprecision(2) << complete_time[i];
        std::string recordTime = str.str();
		recordTexture.loadFromRenderedText(recordTime+" s",White,REND,Font);
		recordTexture.render(SCREEN_WIDTH/4*3,SCREEN_HEIGHT/8*(i+3),SCREEN_WIDTH/4,SCREEN_HEIGHT/8,REND);
        
	}
    for(int i=0;i<5;i++){
        recordTexture.loadFromRenderedText("RANK"+std::to_string(ranking[i]),White,REND,Font2);
		recordTexture.render(0,SCREEN_HEIGHT/8*(i+3),SCREEN_WIDTH/4,SCREEN_HEIGHT/8,REND);
        recordTexture.loadFromRenderedText(UserName[i],White,REND,Font2);
        recordTexture.render(SCREEN_WIDTH*0.3,SCREEN_HEIGHT/8.0*(i+3),SCREEN_WIDTH*0.4*(UserName[i].length()/8.0),SCREEN_HEIGHT/8.0,REND);
    }
}

void Record::setRecordTime(double timing){
    bool find = true;
    for(int i=0;find && i<5;++i){
        if(complete_time[i]==0 || complete_time[i]>timing){
            for(int j=4;j>i;--j){
                complete_time[j]=complete_time[j-1];
                UserName[j]=UserName[j-1];
            }
            complete_time[i]=timing;
            new_record_ind=i;
            find = false;
        }
    }
}

void Record::setRecordName(std::string name){
    if(new_record_ind >= 0){
        UserName[new_record_ind]=name;
        new_record_ind = -1;
    }
}

#endif