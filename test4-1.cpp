#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <utility>  
#include <cstdlib>


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const Uint8* keyarr = SDL_GetKeyboardState(NULL);

int segement_length = 1600;
int road_width = 2400;

SDL_Window * window = NULL;
SDL_Surface * surface = NULL;
SDL_Texture * texture = NULL;
SDL_Renderer * renderer = NULL;
//Globally used font
TTF_Font* gFont = NULL;




SDL_Color White= {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};
SDL_Color Black= {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Dark_Green= {.r = 0, .g = 102, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Light_Green= {.r = 0, .g = 255, .b = 0, .a = SDL_ALPHA_OPAQUE};
SDL_Color Dark_Road= {.r = 100, .g = 100, .b = 100, .a = SDL_ALPHA_OPAQUE};
SDL_Color Light_Road= {.r = 105, .g = 105, .b = 105, .a = SDL_ALPHA_OPAQUE};
SDL_Color Blue_Sky= {.r = 53, .g = 81, .b = 92, .a = SDL_ALPHA_OPAQUE};
SDL_Color Water= {.r = 15, .g = 94, .b = 156, .a = SDL_ALPHA_OPAQUE};

//Texture wrapper class
class LTexture
{
    public:
        //Initializes variables
        LTexture();

        //Deallocates memory
        ~LTexture();

        //Loads image at specified path
        bool loadFromFile( SDL_Renderer * renderer , std::string path );
        
        //Creates image from font string
        bool loadFromRenderedText( SDL_Renderer * renderer ,std::string textureText, SDL_Color textColor );

        //Deallocates texture
        void free();

        //Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Set blending
        void setBlendMode( SDL_BlendMode blending );

        //Set alpha modulation
        void setAlpha( Uint8 alpha );
        
        //Renders texture at given point
        void render(SDL_Renderer * renderer , int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        //Gets image dimensions
        int getWidth();
        int getHeight();

    private:
        //The actual hardware texture
        SDL_Texture* mTexture;

        //Image dimensions
        int mWidth;
        int mHeight;
};


//Rendered texture
LTexture  gTextTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( SDL_Renderer * renderer ,std::string path )
{
	//Get rid of preexisting texture
	free();

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
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText( SDL_Renderer * renderer, std::string textureText, SDL_Color textColor )
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    
    //Return success
    return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( SDL_Renderer * renderer,int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( renderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

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

enum Car_Type {
    AE86, R7X
};

class Car3D{
    public:
        bool main_car;

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

        void resolve_file_attributes(){
            switch(type){
                case AE86:
                picture_width = 100;
                picture_height = 100;
                file_pos = "test_race_img/AE86_cropped.png";
                car_texture = loadTexture(renderer, file_pos);
                break;

                case R7X:
                picture_width = 100;
                picture_height = 100;
                file_pos = "test_race_img/R7X_cropped.png";
                car_texture = loadTexture(renderer, file_pos);
                break;
            }
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

enum Obstacle_Type {
    cone, rock
};

class Obstacle3D{
public :
    double x, y, z, scale, original_scale = 2;//cooridinate in 3D space
    int segment_number_position;
    int screenX, screenY;//cooridinate on screen;
    int picture_width, picture_height;
    Obstacle_Type type = cone;
    std::string file_pos;
    SDL_Texture * obstacle_texture;

    Obstacle3D(){
        x=0,y=0, segment_number_position=0;
        type = cone;
        resolve_file_attributes();
    }
    Obstacle3D(int x, int y, int segment_number_position, Obstacle_Type type){
        this->x=x, this->y=y, this->segment_number_position=segment_number_position, this->type = type;
        z = segment_number_position * segement_length;
        resolve_file_attributes();
    }

    void change_type( Obstacle_Type type){
        this->type = type;
        resolve_file_attributes();
    }

    void resolve_file_attributes(){
        switch(type){
            case cone: 
                file_pos = "test_race_img/cone.png";
                picture_width = 100, picture_height = 128;
                obstacle_texture = loadTexture(renderer, file_pos);
                break;
        }
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
    int get_segment_number_position(){return segment_number_position;}


};

struct Obstacle_build{
    double x, y, segment_number_position;
    Obstacle_Type type;
};



class Map{
public :
    std::string name;
    int Line_number, Node_number, Obstacle_number;
    std::pair<std::pair<int,int>,double> * Nodes;
    Obstacle_build * Obstacle_arr;
    Line3D * lines;
    Obstacle3D * Obstacles;
    int * Obstacles_Location;
    // Nodes consists <line number, curve>, meaning from last line number(default 0) to this line number, the curve would be <curve> 
    Map(std::string name, int Line_number , int Node_number, std::pair<std::pair<int,int>,double> * Nodes , int Obstacle_number , Obstacle_build * Obstacle_arr ){
        this->name = name ,this->Line_number = Line_number, this->Nodes = Nodes, this->Node_number = Node_number, this->Obstacle_number = Obstacle_number, this->Obstacle_arr = Obstacle_arr;
        lines = new Line3D[Line_number];
        Obstacles = new Obstacle3D[Obstacle_number];
        Obstacles_Location = new int[Line_number];

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
    int get_obstacle_number(){return Obstacle_number;}
    Obstacle3D * get_Obstacles(){return Obstacles;}
    int * get_obstacles_location(){return Obstacles_Location;}


    ~Map(){
        delete [] Nodes;
        delete Nodes;
        delete [] lines;
        delete lines;
        delete [] Obstacles;
        delete Obstacles;
        delete [] Obstacles_Location;
        delete Obstacles_Location;
    }

    

};


bool init(){
    //Initialization flag
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO) < 0){
        std::cout<<"SDL couldn't Initialize, Error : "<<SDL_GetError();
        success = false;
    }
    else{
        //Create window
        window = SDL_CreateWindow( "test4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get Render
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		}
        //Initialize SDL TTF
        if( TTF_Init() == -1 )
        {
            printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
            success = false;
        }

        //Initialize SDL image loader for png
        int imgFlags = IMG_INIT_PNG;//specify using png 

        if( !( IMG_Init( imgFlags ) & imgFlags ) )
        {
            printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
            success = false;
        }
    }
    return success;
}

bool loadmedia(){
    bool success = 1;
    return success; 
}

void close(){
    //Free loaded images
    gTextTexture.free();

    //Free global font
    TTF_CloseFont( gFont );
    gFont = NULL;
    SDL_DestroyRenderer(renderer);
    //SDL_DestroyWindowSurface(window);
    SDL_DestroyWindow(window);
}

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

void draw_obstacle(SDL_Renderer * renderer, Obstacle3D * obstacle ){
    SDL_Rect ObstacleViewport;
    ObstacleViewport.w = (int) obstacle->picture_width * obstacle->original_scale * obstacle->scale * 1500;
    ObstacleViewport.h = (int) obstacle->picture_height * obstacle->original_scale * obstacle->scale * 1500;
    ObstacleViewport.x = obstacle->screenX - ObstacleViewport.w / 2 ;
    ObstacleViewport.y = obstacle->screenY - ObstacleViewport.h / 2;

    //std::cout<<obstacle->segment_number_position<<' '<<ObstacleViewport.w<<' '<<ObstacleViewport.h<<' '<<ObstacleViewport.x<<' '<<ObstacleViewport.y<<'\n';

    SDL_RenderSetViewport( renderer, &ObstacleViewport );
    SDL_RenderCopy( renderer, obstacle->obstacle_texture, NULL, NULL );     
    SDL_RenderSetViewport( renderer, NULL );   
}

void Car_Obstacle_Collision(Car3D * car, Map * map, int detect_segment_range){

    double distance_x = 300, distance_z = 1000, shift_z = -10 * segement_length ; // somehow the z coordinate of obstacle is delayed 10 segment_length
    int start_pos = car->get_z()/segement_length;
    int * obstacles_location = map->get_obstacles_location();
    for(int i = start_pos ; i < start_pos + detect_segment_range ; i++){
        if(obstacles_location[i% map->Line_number] != -1){
            //std::cout<<car->get_x() - map->Obstacles[ obstacles_location[i] ].get_x()<<' '<<car->get_z() - map->Obstacles[ obstacles_location[i] ].get_z()<<'\n';
            //std::cout<<car->get_z()<<' ';
            if(car->get_x() - map->Obstacles[ obstacles_location[i] ].get_x() < distance_x && car->get_x() - map->Obstacles[ obstacles_location[i% map->Line_number] ].get_x() > -1 * distance_x && car->get_z() - map->Obstacles[ obstacles_location[i% map->Line_number] ].get_z() < distance_z + shift_z  && car->get_z() - map->Obstacles[ obstacles_location[i% map->Line_number] ].get_z() > -1 * distance_z + shift_z ){
                //std::cout<<"in range"<<'\n';
                car->increment_vz( -1 * car->get_speed_vz() );
            }             
        }
    }

}

void draw_words(SDL_Renderer * renderer, std::string ss, int screenx, int screeny){
    /*
    SDL_Rect BottomRightViewport;
    BottomRightViewport.x = SCREEN_WIDTH *1 /2;
    BottomRightViewport.y = SCREEN_WIDTH * 1 / 2;
    BottomRightViewport.w = SCREEN_WIDTH / 3;
    BottomRightViewport.h = SCREEN_HEIGHT / 3;
    SDL_RenderSetViewport( renderer, &BottomRightViewport );
    */
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
        if( !gTextTexture.loadFromRenderedText( renderer,  ss, textColor ) )
        {
            printf( "Failed to render text texture!\n" );
        }
    }
    gTextTexture.render(renderer, ( screenx - gTextTexture.getWidth()/ 2  ) , ( screeny - gTextTexture.getHeight()/ 2  ) );
    gTextTexture.free();
    //SDL_RenderSetViewport( renderer, NULL );

}

void draw_scene(SDL_Renderer * renderer, Map * map, Camera3D * cam, int lines_drawn){
    int start_pos = cam->z / segement_length;
    int total_lines = map->Line_number;
    double x = 0, dx = 0;

    Obstacle3D * Obstacles = map->Obstacles;
    int * Obstacles_Location = map->Obstacles_Location;
    Obstacle3D * curr_obstacle = NULL;
    SDL_Rect ObstacleViewport;

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
        draw_quad(renderer, 0, prev_line->screenY, SCREEN_WIDTH, 0,curr_line->screenY, SCREEN_WIDTH, Water);
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width*1.2 , curr_line->screenX ,curr_line->screenY, curr_line->width*1.2 , rumble);
    
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width , curr_line->screenX ,curr_line->screenY, curr_line->width , road);
        //draw road segment line
        if((i/3)%6 < 3){
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width/32 , curr_line->screenX ,curr_line->screenY, curr_line->width/32 , White);
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

    //draw sky
    draw_quad(renderer, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, Blue_Sky); 


    cam->z+=cam->vz;
    
    
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

int WinMain(){

    if( !init() ){
        std::cout<<"Failed to Initialize\n";
    }
    else{
        if( !loadmedia() ){
            std::cout<<"Failed to load media\n";
        }
        else{
            bool quit = false;

            //Event Handler
            SDL_Event e;

            //Create Map
            int Node_number = 11;
            std::pair<std::pair<int,int>,double> * Nodes = new std::pair<std::pair<int,int>,double>[Node_number];
            Nodes[0] = std::make_pair( std::make_pair(100,300) , 0.2 );
            Nodes[1] = std::make_pair( std::make_pair(500,1000) , -0.5 );
            Nodes[2] = std::make_pair( std::make_pair(1000,1200) , 1 );
            Nodes[3] = std::make_pair( std::make_pair(1400,1700) , -0.4 );
            Nodes[4] = std::make_pair( std::make_pair(1800,2100) , -1.2 );
            Nodes[5] = std::make_pair( std::make_pair(2100,3000) , 1.2 );
            Nodes[6] = std::make_pair( std::make_pair(3100,3200) , -1.2 );
            Nodes[7] = std::make_pair( std::make_pair(3300,3400) , 1.2 );
            Nodes[8] = std::make_pair( std::make_pair(3500,3700) , -1.2 );
            Nodes[9] = std::make_pair( std::make_pair(3800,3900) , 1.2 );
            Nodes[10] = std::make_pair( std::make_pair(4000,6000) , -1.2 );

            //Create Obstacle
            int Obstacle_number = 1000;
            srand (time(NULL));
            Obstacle_build * obstacle_details = new Obstacle_build[Obstacle_number];
            for(int i = 0 ; i < Obstacle_number ; i++){
                obstacle_details[i].type = cone;
                obstacle_details[i].x = ( (double) rand() / RAND_MAX * 2 - 1) * road_width ;
                //std::cout<<obstacle_details[i].x<<' ';
                obstacle_details[i].y = 0;
                obstacle_details[i].segment_number_position = i*25 ;
            }
            

            Map * test_map = new Map("Test_Map",10000,5,Nodes, Obstacle_number, obstacle_details);
            

            //Create Camera
            Camera3D * cam = new Camera3D(0,1550,100,5,0,0);

            Car3D * car_main = new Car3D(R7X,true);

            //Clear renderer
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);


            //int k = 0;
            //While app is running
            while( !quit ){
                //calculate time
                Uint32 time_start = SDL_GetTicks();
                //Handle event on queue

                while( SDL_PollEvent( &e ) != 0){
                    if( e.type == SDL_QUIT ){
                        quit = true;
                    }
                    
                    else if(e.type == SDL_KEYDOWN){
                        SDL_PumpEvents();

                        if(keyarr[SDL_SCANCODE_Q]){
                            cam->D+=0.1;
                            //std::cout<<cam->D<<'\n';
                        }
                            
                        if(keyarr[SDL_SCANCODE_E]){
                            cam->D-=0.1;
                            //std::cout<<cam->D<<'\n';
                        }
                        if(keyarr[SDL_SCANCODE_W]){
                            cam->y+=5;
                            std::cout<<cam->y<<'\n';
                        }
                            
                        if(keyarr[SDL_SCANCODE_S]){
                            cam->y-=5;
                            std::cout<<cam->y<<'\n';
                        }
                    }
                    else{
                        //do keyboard or mouse interaction event
                    }

                }
                if(keyarr[SDL_SCANCODE_LEFT])
                    car_main->increment_x( car_main->get_speed_vz()*-0.04 );
                if(keyarr[SDL_SCANCODE_RIGHT])
                    car_main->increment_x( car_main->get_speed_vz()*0.04 );
                if(keyarr[SDL_SCANCODE_UP])
                    car_main->increment_vz( 5 );
                if(keyarr[SDL_SCANCODE_DOWN])
                    car_main->increment_vz( -5 );


                car_main->increment_x( -1 * car_main->get_speed_vz() * 0.01 * test_map->get_curve((int) car_main->get_z() / segement_length ) );
                //car_main->x-=car_main->vz * 0.01 * test_map->get_curve((int) car_main->z / segement_length );
                
        

                //Test
                SDL_SetRenderDrawColor(renderer, Blue_Sky.r,Blue_Sky.g,Blue_Sky.b,Blue_Sky.a);
                SDL_RenderClear(renderer);
                //SDL_RenderDrawLine(renderer, 0,0,100,100);
                //draw_quad(renderer, 0,100,100,500,500,200,Light_Green);
                

                //Draw Scene
                draw_scene(renderer, test_map, cam ,300);

                //draw speed
                std::ostringstream stream;
                stream << std::fixed << std::setprecision(2) << car_main->vz;
                std::string stringValue = stream.str();
                draw_words(renderer, stringValue , SCREEN_WIDTH*9/10, SCREEN_HEIGHT*9/10);
                stream.str("");
                stream.clear();

                //draw progress
                stream << std::fixed << std::setprecision(2) << car_main->z/(test_map->Line_number*segement_length)*100<<'%';
                stringValue = stream.str();
                //std::cout<<stream.str()<<'\n';
                draw_words(renderer, stringValue , SCREEN_WIDTH*1/10, SCREEN_HEIGHT*9/10);
                stream.str("");
                stream.clear();
                
                //draw cars, main car
                draw_cars(renderer, cam , car_main);
                car_main->x -= test_map->lines[(int) car_main->z/segement_length].curve * car_main->vz * 0.001;

                Car_Obstacle_Collision(car_main,test_map,30);
                
                //draw obstacles
                //draw_obstacles(renderer, cam, test_map);

                //Present
                SDL_RenderPresent(renderer);
                
                
                //cap framerate
                framerate_cap(time_start, 60);

                
            }
            delete test_map;
            delete cam;
            
        }
    }
    close();


    return 0;
}

