#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <utility>  
#include <cstdlib>

#include "LTexture.cpp"

/*const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;*/
const Uint8* keyarr = SDL_GetKeyboardState(NULL);

int segement_length = 1600;
int road_width = 2400;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window * window = NULL;
SDL_Surface * surface = NULL;
SDL_Texture * texture = NULL;
SDL_Renderer * renderer = NULL;
//Globally used font
TTF_Font* gFont = NULL;

//Sound effect
//The music that will be played
Mix_Music* music = NULL;

//The sound effects that will be used
Mix_Chunk* crash = NULL;
Mix_Chunk* accelerating = NULL;
Mix_Chunk* accelerating_fast = NULL;
Mix_Chunk* decelerating = NULL;
Mix_Chunk* curse = NULL;


SDL_Color White= {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};
SDL_Color Black= {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Dark_Green= {.r = 0, .g = 102, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Light_Green= {.r = 0, .g = 255, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Dark_Road= {.r = 100, .g = 100, .b = 100, .a = SDL_ALPHA_OPAQUE};
SDL_Color Light_Road= {.r = 105, .g = 105, .b = 105, .a = SDL_ALPHA_OPAQUE};
SDL_Color Blue_Sky= {.r = 53, .g = 81, .b = 92, .a = SDL_ALPHA_OPAQUE};
SDL_Color Water= {.r = 15, .g = 94, .b = 156, .a = SDL_ALPHA_OPAQUE};

const int Num_Of_Cars = 2;
enum Car_Type {
    AE86, R7X
};

std::string Car_File_locations[Num_Of_Cars] = {"test_race_img/AE86_cropped.png","test_race_img/R7X_cropped.png"};
SDL_Texture ** Car_Textures = new SDL_Texture*[Num_Of_Cars];

const int Num_Of_Obstacles = 4;
enum Obstacle_Type {
    cone, broken_cone, rock, finish_flag
};
std::string Obstacle_File_locations[Num_Of_Obstacles] = {"test_race_img/cone.png","test_race_img/broken_cone.png","test_race_img/rock_cropped.png","test_race_img/finish_flag_cropped.png"};
SDL_Texture ** Obstacle_Textures = new SDL_Texture*[Num_Of_Obstacles];


const int Num_Of_Maps = 3;
enum Map_Type {
    Seaways_Dawn, Seaways_Noon, Seaways_Dusk
};
std::string Background_File_locations[Num_Of_Maps] = {"test_race_img/dusk.png","test_race_img/dusk.png","test_race_img/dusk.png"};
SDL_Texture * Background_Texture;

const int Num_of_Musics = 3;
enum Music_type {
    Deja_Vu, Forever_Young, Tokyo_Drift
};
std::string Music_File_Location[Num_of_Musics] = {
    "testmusic/Deja_Vu_fixed.mp3",
    "testmusic/Forever Young - Symbol.mp3",
    "testmusic/Tokyo Drift - If You Were Here  Remix.mp3"
};


enum Car_Status{
    Intact, In_Water, On_Rock
};

//Rendered texture
LTexture  gTextTexture;


SDL_Texture* loadTexture( SDL_Renderer * renderer, std::string path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

class Camera3D{
    public:
        double x, y ,z, D, vz, vx;
        Camera3D(){
            x=0, y=1000, z=0, D=5, vx = 0, vz = 0;
        }
        Camera3D(double x, double y, double z, double D, double vx,double vz){
            this->x=x, this->y=y, this->z=z, this->D=D, this->vx=vx, this->vz=vz;
        }
};



class Line3D{
    public :
        double x,y,z; //3D center coordinate of line
        int screenX,screenY,width; //line coordinate on screen 
        double scale, curve;

        Line3D(){
            x=0, y=0, z=0, scale=0, curve=0;
        }
        //3D coordinate to screen coordinate
        void project(Camera3D * cam){
            double delta_z = (z - cam->z <= 10 ? 10 : z - cam->z);
            scale = cam->D/(z-cam->z);
            screenX = (int) ((1 + scale*( x - cam->x ))*SCREEN_WIDTH / 2);
            screenY = (int) ((1 - scale*( y - cam->y ))*SCREEN_HEIGHT / 2);
            width = (int) (scale * road_width * SCREEN_WIDTH / 2);
        }
};

class Finish_Line3D{
    public:
        double x, y ,z,  vz, vx, original_scale = 2;
        int picture_width, picture_height;
};

class Obstacle3D{
public :
    double x, y, z, scale, original_scale = 2;//cooridinate in 3D space
    int segment_number_position;
    int screenX, screenY;//cooridinate on screen;
    int picture_width, picture_height;
    bool can_collide = 1;
    Obstacle_Type type = cone;
    std::string file_pos;
    SDL_Texture * obstacle_texture;

    Obstacle3D(){
        x=0,y=0, segment_number_position=0;
        type = cone;
        resolve_file_attributes();
    }
    Obstacle3D(int x, int y, int segment_number_position, Obstacle_Type type, double original_scale){
        this->x=x, this->y=y, this->segment_number_position=segment_number_position, this->type = type, this->original_scale = original_scale ;
        z = segment_number_position * segement_length;
        resolve_file_attributes();
    }

    void change_type( Obstacle_Type type){
        this->type = type;
        resolve_file_attributes();
    }

    void change_can_collide(bool can_collide){
        this->can_collide = can_collide;
    }

    void resolve_file_attributes(){
        switch(type){
            case cone: 
                picture_width = 100, picture_height = 128;
                break;
            case rock:
                picture_width = 137, picture_height = 100;
                break;
            case finish_flag:
                picture_width = 100, picture_height = 100;
                break;
        }
        obstacle_texture = Obstacle_Textures[(int) type];
    }    

    void project(Camera3D * cam){
        scale = cam->D/(segment_number_position*segement_length - cam->z);
        //scale = 1;
        screenX = (int) ((1 + scale*( x - cam->x ))*SCREEN_WIDTH / 2);
        screenY = (int) ((1 - scale*( y - cam->y ))*SCREEN_HEIGHT / 2);
        //std::cout<<y - cam->y<<'\n';
    }

    double get_x(){return x;}
    double get_y(){return y;}
    double get_z(){return segment_number_position*segement_length;}
    Obstacle_Type get_obstacle_type(){return type;}
    bool get_can_collide(){return can_collide;}
    int get_segment_number_position(){return segment_number_position;}


};

class Background{
    int picture_width, picture_height;
    std::string file_pos;
    Map_Type type;

    void resolve_file_attributes(){
        switch(type){
            case Seaways_Dawn:
                picture_width = SCREEN_WIDTH, picture_height = SCREEN_HEIGHT;
                break;
            case Seaways_Noon:
                picture_width = SCREEN_WIDTH, picture_height = SCREEN_HEIGHT;
                break;
            case Seaways_Dusk:
                picture_width = SCREEN_WIDTH, picture_height = SCREEN_HEIGHT;
                break;
        };
    }
public:
    Background(Map_Type type){
        this->type = type;
        resolve_file_attributes();
    }
    int get_picture_width(){return picture_width;}
    int get_picture_height(){return picture_height;}
};

struct Obstacle_build{
    double x, y, segment_number_position, original_scale;
    Obstacle_Type type;
};

class Map{
public :
    std::string name;
    int Line_number, Node_number, Obstacle_number, Finish_Line_segement_number;
    //std::pair<std::pair<int,int>,double> * Nodes;
    //Obstacle_build * Obstacle_arr;
    Line3D * lines;
    Obstacle3D * Obstacles;
    int * Obstacles_Location;
    Background * background;
    Map_Type type;
    // Nodes consists <line number, curve>, meaning from last line number(default 0) to this line number, the curve would be <curve> 
    Map(std::string name, int Line_number , int Node_number, std::pair<std::pair<int,int>,double> * Nodes , int Obstacle_number , Obstacle_build * Obstacle_arr , int Finish_Line_segement_number, Map_Type type){
        this->name = name ,this->Line_number = Line_number, /*this->Nodes = Nodes,*/ this->Node_number = Node_number, this->Obstacle_number = Obstacle_number, /*this->Obstacle_arr = Obstacle_arr,*/ this->Finish_Line_segement_number = Finish_Line_segement_number, this->type = type;
        lines = new Line3D[Line_number];
        Obstacles = new Obstacle3D[Obstacle_number];
        Obstacles_Location = new int[Line_number];
        background = new Background(type);

        for(int i = 0 ; i < Line_number ; i++){
            lines[i].z = segement_length*i;
            Obstacles_Location[i] = -1;
        }

        for(int i = 0 ; i < Node_number ; i++){
            for(int j = Nodes[i].first.first ; j < Nodes[i].first.second ; j++){
                lines[j].curve = Nodes[i].second;
            }
        }

        for(int i = 0 ; i < Obstacle_number ; i++){
            Obstacles[i].x = Obstacle_arr[i].x;
            Obstacles[i].y = Obstacle_arr[i].y;
            Obstacles[i].segment_number_position = Obstacle_arr[i].segment_number_position;
            Obstacles_Location[ Obstacles[i].segment_number_position ] = i;
            Obstacles[i].change_type( Obstacle_arr[i].type );
            Obstacles[i].original_scale = Obstacle_arr[i].original_scale;
        }
    };

    double get_curve(int line_position ){
        if(line_position >= Line_number || line_position < 0){
            std::cout<<"Error, invalid line position\n";
            return 0;
        }
        else{
            return lines[line_position].curve;
        }
    }

    std::string get_name_of_map(){return name;}
    Map_Type get_map_type(){return type;}
    Background * get_background(){return background;}
    int get_obstacle_number(){return Obstacle_number;}
    Obstacle3D * get_Obstacles(){return Obstacles;}
    int * get_obstacles_location(){return Obstacles_Location;}


    ~Map(){
        //delete [] Nodes;
        //delete Nodes;
        delete [] lines;
        //delete lines;
        delete [] Obstacles;
        //delete Obstacles;
        delete [] Obstacles_Location;
        //delete Obstacles_Location;
    }

    

};


class Car3D{
    public:
        bool main_car;
        Car_Status car_status = Intact;

        double x, y ,z,  vz, vx, original_scale = 2;
        int picture_width, picture_height;
        Car_Type type;
        std::string file_pos;

        int screenX,screenY;
        double scale;

        SDL_Texture * car_texture;


        Car3D(Car_Type type , bool main_car){
            x=0, y=0, z=1650, vx = 0, vz = 0;
            this->type = type, this->main_car = main_car;
            resolve_file_attributes();
        }        
        Car3D(double x, double y, double z, double vx,double vz, Car_Type type,  double original_scale,  bool main_car){
            this->x=x, this->y=y, this->z=z,  this->vx=vx, this->vz=vz, this->type = type,this->original_scale = original_scale, this->main_car = main_car;
            resolve_file_attributes();
        }

        void change_type( Car_Type type){
            this->type = type;
            resolve_file_attributes();
        }

        Car_Status is_car_intact(){
            return car_status;
        }

        void car_on_rock(){
            car_status = On_Rock;
        }

        void car_in_water(){
            car_status = In_Water;
        }


        double get_speed_vx(){return vx;}
        double increment_vx(double delta_vx){
            vx += delta_vx;
            return vx;
        }
        double get_x(){return x;}
        double increment_x(double delta_x){
            x += delta_x;
            return x;
        }

        double get_speed_vz(){return vz;}
        double increment_vz(double delta_vz){
            vz += delta_vz;
            vz = vz>0 ? vz : 0;
            return vz;
        }

        double get_z(){return z;}
        double increment_z(double delta_z){
            z += delta_z;
            return z;
        }

        double accelerate(){
            vz += 5;
            return vz;
        }

        double decelerate(){
            vz -= 25;
            vz = ( vz>=0 ? vz : 0 );
            return vz;
        }

        double move_left(){
            x += (atan(vz/100000)+M_PI/2)*-25;
            //x += log2(vz+1) * -1;
            return x;
        }
        double move_right(){
            x += (atan(vz/100000)+M_PI/2)*25;
            //x += log2(vz+1);
            return x;
        }

        void turn(Map * map){
            //car_main->increment_x( -1 * car_main->get_speed_vz() * 0.01 * map->get_curve((int) car_main->get_z() / segement_length ) );
            x += -1 * log2(vz+1) * 2 * map->get_curve( (int) (z / segement_length) );
        }

        void resolve_file_attributes(){
            switch(type){
                case AE86:
                picture_width = 100;
                picture_height = 100;
                break;

                case R7X:
                picture_width = 100;
                picture_height = 100;   
                break;
            }
            car_texture = Car_Textures[(int) type];
        }

        void project(Camera3D * cam){
            
            x+=vx;
            z+=vz;

            if(main_car==1){cam->x+=vx; cam->z+=vz;}
            scale = cam->D/(z-cam->z)*0.1;
            //scale = 0.01;
            screenX = (int) ((1 + scale*( x - cam->x ))*SCREEN_WIDTH / 2);
            screenY = (int) ((1 - scale*( y - cam->y ))*SCREEN_HEIGHT / 2);
            //screenY = SCREEN_HEIGHT *4/5 ;
            //std::cout<<screenX<<' '<<scale*( y - cam->y )<<'\n';
        }
};


Map * map;
Camera3D * cam;
Car3D * car_main;


//(x1,y1)corresponds to the coordinate of the line center on screen, w1 is the width of line
void draw_quad(SDL_Renderer * renderer, int x1, int y1, int w1, int x2, int y2, int w2, SDL_Color color){
    if(y1==y2){
        SDL_SetRenderDrawColor(renderer, color.r, color.g,color.b,color.a);
        SDL_RenderDrawLine(renderer, x1,y1,x2,y2);
    }
    else{        
        float p1x = (float) ( x1-w1>0 ? x1-w1 : 0 );
        float p1y = (float) y1;
        float p2x = (float) ( x1+w1<SCREEN_WIDTH ? x1+w1 : SCREEN_WIDTH - 1 );
        float p2y = (float) y1;
        float p3x = (float) ( x2+w2<SCREEN_WIDTH ? x2+w2 : SCREEN_WIDTH - 1 );
        float p3y = (float) y2;
        float p4x = (float) ( x2-w2>0 ? x2-w2 : 0 );
        float p4y = (float) y2;
        std::vector< SDL_Vertex > verts1 =
        {
            { SDL_FPoint{ p1x, p1y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ p2x, p2y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ p3x, p3y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
        };
        SDL_RenderGeometry(renderer, NULL,verts1.data(), verts1.size(), NULL,0 );
        std::vector< SDL_Vertex > verts2 =
        {
            { SDL_FPoint{ p1x, p1y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ p3x, p3y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ p4x, p4y }, SDL_Color{ color.r, color.g, color.b, color.a }, SDL_FPoint{ 0 }, },
        };
        SDL_RenderGeometry(renderer, NULL,verts2.data(), verts2.size(), NULL,0 );   
    }
}


void draw_cars(SDL_Renderer * renderer,  Camera3D * cam, Car3D * car){

    if( car->is_car_intact() == Intact){
        car->project(cam);
        SDL_Rect CarViewport;
        CarViewport.w = (int) car->picture_width*car->original_scale*car->scale*1500;
        CarViewport.h = (int) car->picture_width*car->original_scale*car->scale*1500;
        CarViewport.x = car->screenX - CarViewport.w / 2;
        CarViewport.y = car->screenY - CarViewport.h / 2;

        //std::cout<<CarViewport.w<<' '<<CarViewport.h<<' '<<CarViewport.x<<' '<<CarViewport.y<<'\n';

        SDL_RenderSetViewport( renderer, &CarViewport );
        
        //Render texture to screen
        SDL_RenderCopy( renderer, car->car_texture, NULL, NULL );
        SDL_RenderSetViewport( renderer, NULL );        
    }

}

void draw_obstacle(SDL_Renderer * renderer, Obstacle3D * obstacle ){
    SDL_Rect ObstacleViewport;
    ObstacleViewport.w = (int) obstacle->picture_width * obstacle->original_scale * obstacle->scale * 1500;
    ObstacleViewport.h = (int) obstacle->picture_height * obstacle->original_scale * obstacle->scale * 1500;
    ObstacleViewport.x = obstacle->screenX - ObstacleViewport.w / 2 ;
    ObstacleViewport.y = obstacle->screenY - ObstacleViewport.h / 2 ;

    //std::cout<<obstacle->segment_number_position<<' '<<ObstacleViewport.w<<' '<<ObstacleViewport.h<<' '<<ObstacleViewport.x<<' '<<ObstacleViewport.y<<'\n';

    if(SDL_RenderSetViewport( renderer, &ObstacleViewport ) != 0){
        std::cout<<"SDL Error in draw_obstacle : SDL_RenderSetViewport : "<<SDL_GetError();
    }
    if(SDL_RenderCopy( renderer, obstacle->obstacle_texture, NULL, NULL ) != 0){
        std::cout<<"SDL Error in draw_obstacle : SDL_RenderCopy :"<<SDL_GetError();
    }
    if(SDL_RenderSetViewport( renderer, NULL ) != 0){
        std::cout<<"SDL Error in draw_obstacle : SDL_RenderSetViewport :"<<SDL_GetError();
    }
         
    
}

void draw_background(SDL_Renderer * renderer, Background  * background){
    SDL_Rect ObstacleViewport;
    ObstacleViewport.w = (int) background->get_picture_width();
    ObstacleViewport.h = (int) background->get_picture_height();
    ObstacleViewport.x = 0;
    ObstacleViewport.y = 0;

    if(SDL_RenderSetViewport( renderer, &ObstacleViewport ) != 0){
        std::cout<<"SDL Error in draw_obstacle : SDL_RenderSetViewport : "<<SDL_GetError();
    }
    if(SDL_RenderCopy( renderer, Background_Texture, NULL, NULL ) != 0){
        std::cout<<"SDL Error in draw_obstacle : SDL_RenderCopy :"<<SDL_GetError();
    }
    if(SDL_RenderSetViewport( renderer, NULL ) != 0){
        std::cout<<"SDL Error in draw_obstacle : SDL_RenderSetViewport :"<<SDL_GetError();
    }
}

void Car_Obstacle_Collision(Car3D * car, Map * map, int detect_segment_range){

    double distance_x = 300, distance_z = 1000, shift_z = -10 * segement_length ; // somehow the z coordinate of obstacle is delayed 10 segment_length
    int start_pos = car->get_z()/segement_length;
    int * obstacles_location = map->get_obstacles_location();
    for(int i = start_pos ; i < start_pos + detect_segment_range ; i++){
        if(obstacles_location[i% map->Line_number] != -1 && map->Obstacles[ obstacles_location[i] ].get_can_collide() == 1){
            if(car->get_x() - map->Obstacles[ obstacles_location[i] ].get_x() < distance_x && car->get_x() - map->Obstacles[ obstacles_location[i% map->Line_number] ].get_x() > -1 * distance_x && car->get_z() - map->Obstacles[ obstacles_location[i% map->Line_number] ].get_z() < distance_z + shift_z  && car->get_z() - map->Obstacles[ obstacles_location[i% map->Line_number] ].get_z() > -1 * distance_z + shift_z ){
                car->increment_vz( -1 * car->get_speed_vz() );

                if(map->Obstacles[ obstacles_location[i] ].get_obstacle_type() == cone){
                    map->Obstacles[ obstacles_location[i] ].change_type(broken_cone);
                    map->Obstacles[ obstacles_location[i] ].change_can_collide(false);
                    Mix_PlayChannel(0,crash,0);
                }
                else if(map->Obstacles[ obstacles_location[i] ].get_obstacle_type() == rock){
                    Mix_PlayChannel(0,crash,0);
                    car->car_on_rock();
                }
            }             
        }
    
    }
}

bool Reach_Finish(Car3D * car, Map * map){
    if(car->get_z() >= map->Finish_Line_segement_number*segement_length){
        car->increment_vz(-1 * car->get_speed_vz()); 
        return 1;
    }
    else{return 0;}
}

void Fell_into_Ocean(Car3D * car, Map * map){
    if(car->get_x() > road_width*1.25 || car->get_x() < -1 * road_width * 1.25){
        car->increment_vz( -1 * car->get_speed_vz() );
        car->car_in_water();
    }
}

void draw_words(SDL_Renderer * renderer, std::string ss, int screenx, int screeny){
    gFont = TTF_OpenFont( "ttf_fonts/ark-pixel-10px-monospaced-zh_tw.ttf", 10 );
    if( gFont == NULL )
    {
        printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Render text
        SDL_Color textColor = { 0, 0, 0 };
        TTF_SetFontSize(gFont, 50);
        if( !gTextTexture.loadFromRenderedText( ss, textColor , renderer, gFont) )
        {
            printf( "Failed to render text texture!\n" );
        }
    }
    gTextTexture.render(( screenx - gTextTexture.getWidth()/ 2  ) , ( screeny - gTextTexture.getHeight()/ 2  ) , renderer );
    gTextTexture.free();
}

Uint32 show_time(SDL_Renderer * renderer ,Uint32 start_time, Uint32 curr_time ,bool game_started){
    std::ostringstream stream;
    Uint32 elapsed_time = 0;
    if(game_started == true){
        elapsed_time = curr_time - start_time;
    }
    stream << std::fixed << std::setprecision(3) << (double)elapsed_time/1000 <<"s";
    std::string stringValue = stream.str();
    stringValue = stream.str();
    draw_words(renderer, stringValue , SCREEN_WIDTH*9/10, SCREEN_HEIGHT*1/10);
    stream.str("");
    stream.clear();
    return elapsed_time;
}

void show_percentage( SDL_Renderer * renderer, Map * map ,Car3D * car){
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << car->get_z()/(map->Finish_Line_segement_number*segement_length)*100<<'%';
    std::string stringValue = stream.str();
    stringValue = stream.str();
    draw_words(renderer, stringValue , SCREEN_WIDTH*1/10, SCREEN_HEIGHT*9/10);
    stream.str("");
    stream.clear();
}

void show_speed(SDL_Renderer * renderer, Car3D * car){
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << car->get_speed_vz() / 20 <<"km/hr";
    std::string stringValue = stream.str();
    draw_words(renderer, stringValue , SCREEN_WIDTH*9/10, SCREEN_HEIGHT*9/10);
    stream.str("");
    stream.clear();
}

void draw_scene(SDL_Renderer * renderer, Map * map, Camera3D * cam, int lines_drawn){
    int start_pos = cam->z / segement_length;
    int total_lines = map->Line_number;
    double x = 0, dx = 0;

    Obstacle3D * Obstacles = map->Obstacles;
    int * Obstacles_Location = map->Obstacles_Location;
    Obstacle3D * curr_obstacle = NULL;
    SDL_Rect ObstacleViewport;

    //draw background
    draw_background(renderer, map->get_background());
    //draw_quad(renderer, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, Blue_Sky); 


    //draw road
    for(int i = 1+start_pos ; i < lines_drawn+start_pos ; i++){
        
        Line3D * curr_line = &map->lines[i % total_lines];
        Line3D * prev_line = &map->lines[(i-1) % total_lines];

        //camera projection and shift
        cam->x -= x;
        curr_line->project(cam);
        cam->x += x;
        x+=dx; dx+=curr_line->curve;

        //Skip lines that have same y coordinate 
        int skip = 0;
        Line3D * next_line = &map->lines[(i+1) % total_lines];
        next_line->project(cam);



        while(curr_line->screenY==next_line->screenY && curr_line->screenX==next_line->screenX){
            skip++;
            next_line = &map->lines[(i+skip+1) % total_lines];
            cam->x -= x;
            next_line->project(cam);
            cam->x += x;
            x+=dx; dx+=next_line->curve;
        }
        i+=skip;

        //select colors for drawing road
        SDL_Color grass = (i/3)%2 ? Light_Green : Dark_Green;
        SDL_Color rumble = (i/3)%2 ? White : Black;
        SDL_Color road = (i/3)%2 ? Light_Road : Dark_Road;
        SDL_Color road_mid_line = (i/3)%4 < 2 ? Light_Road : Dark_Road;

        //SDL_RenderClear(renderer);
        //draw_quad(renderer, 0, prev_line->screenY, SCREEN_WIDTH, 0,curr_line->screenY, SCREEN_WIDTH, Water);
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width*1.2 , curr_line->screenX ,curr_line->screenY, curr_line->width*1.2 , rumble);
    
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width , curr_line->screenX ,curr_line->screenY, curr_line->width , road);
        //draw road segment line
        if((i/3)%6 < 3){
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width/32 , curr_line->screenX ,curr_line->screenY, curr_line->width/32 , White);
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width/32 , curr_line->screenX ,curr_line->screenY, curr_line->width/32 , White);
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width/32 , curr_line->screenX ,curr_line->screenY, curr_line->width/32 , White);
        }          
        //draw finish line
        
        if(i - map->Finish_Line_segement_number < 8 && i - map->Finish_Line_segement_number > -8 ){
            SDL_Color First = ( i%4 < 2 ? White : Black );
            SDL_Color Second = ( i%4 >= 2 ? White : Black );
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width * 4 / 4 , curr_line->screenX ,curr_line->screenY, curr_line->width * 4 / 4 , First);
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width * 3 / 4 , curr_line->screenX ,curr_line->screenY, curr_line->width * 3 / 4 , Second);
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width * 2 / 4 , curr_line->screenX ,curr_line->screenY, curr_line->width * 2 / 4 , First);
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width * 1 / 4 , curr_line->screenX ,curr_line->screenY, curr_line->width * 1 / 4 , Second);
        }
    } 

 
    //draw obstacles
    x = 0, dx = 0;
    for(int i = 1+start_pos ; i < lines_drawn+start_pos ; i++){
        Line3D * curr_line = &map->lines[i % total_lines];
        Line3D * prev_line = &map->lines[(i-1) % total_lines];

        //camera projection and shift
        cam->x -= x;
        curr_line->project(cam);
        if(Obstacles_Location[i]!=-1){
            curr_obstacle = &Obstacles[ Obstacles_Location[i] ];
            curr_obstacle->project(cam);
            draw_obstacle(renderer, curr_obstacle);
        }
        cam->x += x;
        x+=dx; dx+=curr_line->curve;

        //car and obstacle collision
        

        //Skip lines that have same y coordinate 
        int skip = 0;
        Line3D * next_line = &map->lines[(i+1) % total_lines];
    } 
}

void framerate_cap(Uint32 start, int fps){
    static int cnt = 0;
    double ms_per_frame = 1000/fps;
    double elapsed_time = (double) (SDL_GetTicks() - start);
    if( elapsed_time < ms_per_frame ){
        SDL_Delay( (int) (ms_per_frame - elapsed_time) );
    }
    
    if(cnt==10){std::cout<<elapsed_time<<' ';cnt=0;}
    else{cnt++;}
    
}



//functions for main 
void load_game_media(SDL_Renderer * renderer, Map_Type type){
    crash = Mix_LoadWAV("testmusic/crash.wav");
    if(crash==NULL){ std::cout<<"Mix_LoadWav error\n";}
    decelerating = Mix_LoadWAV("testmusic/car_break.wav");
    accelerating = Mix_LoadWAV("testmusic/car_accelerate.wav");
    accelerating_fast = Mix_LoadWAV("testmusic/car_accelerate_fast.wav");
    switch(type){
        case Seaways_Dawn:
            music = Mix_LoadMUS(Music_File_Location[Deja_Vu].c_str());
            break;
        case Seaways_Noon:
            music = Mix_LoadMUS(Music_File_Location[Forever_Young].c_str());
            break;
        case Seaways_Dusk:
            music = Mix_LoadMUS(Music_File_Location[Tokyo_Drift].c_str());
            break;
    };
    
    if(music==NULL){ std::cout<<"Mix_LoadMUS error : "<<Mix_GetError()<<'\n';}

    //load texture for car and obstacle
    for( int i = 0 ; i < Num_Of_Cars ; i++){Car_Textures[i] = loadTexture(renderer, Car_File_locations[i] );}
    for( int i = 0 ; i < Num_Of_Obstacles ; i++){Obstacle_Textures[i] = loadTexture(renderer, Obstacle_File_locations[i] );}
    //load texture for backgrouind
    Background_Texture = loadTexture(renderer,Background_File_locations[(int)type]);
    if(Background_Texture==NULL){std::cout<<"Unable to Load Background Texture : "<<SDL_GetError();}
    Mix_PlayMusic(music,0);
}

void create_map(Map_Type type, Map * &map){
    //Create Map
    Obstacle_build * obstacle_details;
    switch(type){
        case Seaways_Dawn:
        {
            int Node_number = 11;
            std::pair<std::pair<int,int>,double> * Nodes = new std::pair<std::pair<int,int>,double>[Node_number];
            Nodes[0] = std::make_pair( std::make_pair(100,300) , 0.2 );
            Nodes[1] = std::make_pair( std::make_pair(500,1000) , -0.5 );
            Nodes[2] = std::make_pair( std::make_pair(1000,1200) , 1 );
            Nodes[3] = std::make_pair( std::make_pair(1400,1700) , -0.4 );
            Nodes[4] = std::make_pair( std::make_pair(1800,2100) , -1.2 );
            Nodes[5] = std::make_pair( std::make_pair(2200,3000) , 1.2 );
            Nodes[6] = std::make_pair( std::make_pair(3100,3200) , -1.2 );
            Nodes[7] = std::make_pair( std::make_pair(3300,3400) , 1.2 );
            Nodes[8] = std::make_pair( std::make_pair(3500,3700) , -1.2 );
            Nodes[9] = std::make_pair( std::make_pair(3800,3900) , 1.2 );
            Nodes[10] = std::make_pair( std::make_pair(4000,6000) , -1.2 );

            //Create Obstacle
            int Obstacle_number = 10;
            srand (time(NULL));
            obstacle_details = new Obstacle_build[Obstacle_number];
            //other obstacles
            for(int i = 0 ; i < Obstacle_number - 1 ; i++){
                obstacle_details[i].type = (i%2==0 ? cone : rock) ;
                obstacle_details[i].x = ( (double) rand() / RAND_MAX * 2 - 1) * road_width ;
                //std::cout<<obstacle_details[i].x<<' ';
                obstacle_details[i].y = 0;
                obstacle_details[i].segment_number_position = i*25 ;
                obstacle_details[i].original_scale = 2 ;
            }
            //finish_flag
            obstacle_details[Obstacle_number-1].type = finish_flag ;
            obstacle_details[Obstacle_number-1].x = road_width * 1.3 ;
            obstacle_details[Obstacle_number-1].y = 2000;
            obstacle_details[Obstacle_number-1].segment_number_position = 250 ;
            obstacle_details[Obstacle_number-1].original_scale = 10;

            map = new Map("map",110000,Node_number,Nodes, Obstacle_number, obstacle_details,250,type);
            break;        
        }
        case Seaways_Noon:
        {
            int Node_number = 11;
            std::pair<std::pair<int,int>,double> * Nodes = new std::pair<std::pair<int,int>,double>[Node_number];
            Nodes[0] = std::make_pair( std::make_pair(100,300) , 0.5 );
            Nodes[1] = std::make_pair( std::make_pair(500,1000) , -0.1 );
            Nodes[2] = std::make_pair( std::make_pair(1000,1200) , 1.3 );
            Nodes[3] = std::make_pair( std::make_pair(1400,1700) , -0.2 );
            Nodes[4] = std::make_pair( std::make_pair(1701,1800) , -1.5 );
            Nodes[5] = std::make_pair( std::make_pair(1801,1900) , 1.5 );
            Nodes[6] = std::make_pair( std::make_pair(3100,2000) , -1.5 );
            Nodes[7] = std::make_pair( std::make_pair(2101,2200) , 1.5 );
            Nodes[8] = std::make_pair( std::make_pair(2301,2550) , -1.5 );
            Nodes[9] = std::make_pair( std::make_pair(3800,3900) , 1.2 );
            Nodes[10] = std::make_pair( std::make_pair(4000,6000) , -1.2 );

            //Create Obstacle
            int Obstacle_number = 1000;
            srand (time(NULL));
            obstacle_details = new Obstacle_build[Obstacle_number];
            //other obstacles
            for(int i = 0 ; i < Obstacle_number - 1 ; i++){
                obstacle_details[i].type = (i%2==0 ? cone : rock) ;
                obstacle_details[i].x = ( (double) rand() / RAND_MAX * 2 - 1) * road_width ;
                //std::cout<<obstacle_details[i].x<<' ';
                obstacle_details[i].y = 0;
                obstacle_details[i].segment_number_position = i*30 ;
                obstacle_details[i].original_scale = 2 ;
            }
            //finish_flag
            obstacle_details[Obstacle_number-1].type = finish_flag ;
            obstacle_details[Obstacle_number-1].x = road_width * 1.3 ;
            obstacle_details[Obstacle_number-1].y = 2000;
            obstacle_details[Obstacle_number-1].segment_number_position = 5000 ;
            obstacle_details[Obstacle_number-1].original_scale = 10;

            map = new Map("map",31000,Node_number,Nodes, Obstacle_number, obstacle_details,5000,type);
            break;        
        }
        case Seaways_Dusk:
        {
            int Node_number = 11;
            std::pair<std::pair<int,int>,double> * Nodes = new std::pair<std::pair<int,int>,double>[Node_number];
            Nodes[0] = std::make_pair( std::make_pair(100,300) , 0.5 );
            Nodes[1] = std::make_pair( std::make_pair(500,1000) , -0.1 );
            Nodes[2] = std::make_pair( std::make_pair(1000,1200) , 1.3 );
            Nodes[3] = std::make_pair( std::make_pair(1400,1700) , -0.2 );
            Nodes[4] = std::make_pair( std::make_pair(1701,1800) , -1.5 );
            Nodes[5] = std::make_pair( std::make_pair(1801,1900) , 1.5 );
            Nodes[6] = std::make_pair( std::make_pair(3100,2000) , -1.5 );
            Nodes[7] = std::make_pair( std::make_pair(2101,2200) , 1.5 );
            Nodes[8] = std::make_pair( std::make_pair(2301,2550) , -1.5 );
            Nodes[9] = std::make_pair( std::make_pair(3800,3900) , 1.2 );
            Nodes[10] = std::make_pair( std::make_pair(4000,6000) , -1.2 );

            //Create Obstacle
            int Obstacle_number = 10;
            srand (time(NULL));
            obstacle_details = new Obstacle_build[Obstacle_number];
            //other obstacles
            for(int i = 0 ; i < Obstacle_number - 1 ; i++){
                obstacle_details[i].type = (i%2==0 ? cone : rock) ;
                obstacle_details[i].x = ( (double) rand() / RAND_MAX * 2 - 1) * road_width ;
                //std::cout<<obstacle_details[i].x<<' ';
                obstacle_details[i].y = 0;
                obstacle_details[i].segment_number_position = i*30 ;
                obstacle_details[i].original_scale = 2 ;
            }
            //finish_flag
            obstacle_details[Obstacle_number-1].type = finish_flag ;
            obstacle_details[Obstacle_number-1].x = road_width * 1.3 ;
            obstacle_details[Obstacle_number-1].y = 2000;
            obstacle_details[Obstacle_number-1].segment_number_position = 5000 ;
            obstacle_details[Obstacle_number-1].original_scale = 10;

            map = new Map("map",31000,Node_number,Nodes, Obstacle_number, obstacle_details,5000,type);
            break;        
        }

    };
    delete [] obstacle_details;
    //delete obstacle_details;



}

void create_camera(){cam = new Camera3D(0,1550,100,5,0,0);}

void create_car(){car_main = new Car3D(AE86,true);}

void close_game(){
    Mix_FreeChunk(crash);
    Mix_FreeMusic(music);

    //delete cam;
    //delete map;
    //delete [] Car_Textures;
    //delete [] Obstacle_Textures;

    map = NULL;

    //Free loaded images
    gTextTexture.free();

    //Free global font
    TTF_CloseFont( gFont );
    gFont = NULL;
}


