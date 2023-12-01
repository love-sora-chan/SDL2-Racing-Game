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


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const Uint8* keyarr = SDL_GetKeyboardState(NULL);

int segement_length = 1600;
int road_width = 1200;

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

class Car3D{
    public:
        bool main_car;

        double x, y ,z,  vz, vx, original_scale = 2;
        int picture_width, picture_height;

        std::string file_pos;

        int screenX,screenY;
        double scale;
        Car3D(int picture_width, int picture_height, std::string file_pos , bool main_car){
            x=0, y=0, z=1650, vx = 0, vz = 0;
            this->picture_width=picture_width, this->picture_height = picture_height, this->file_pos = file_pos, this->main_car = main_car;
        }        
        Car3D(double x, double y, double z, double vx,double vz, double original_scale, int picture_width, int picture_height, std::string file_pos , bool main_car){
            this->x=x, this->y=y, this->z=z,  this->vx=vx, this->vz=vz, this->original_scale = original_scale, this->picture_width=picture_width, this->picture_height = picture_height, this->file_pos = file_pos, this->main_car = main_car;
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

class Map{
public :
    std::string name;
    int Line_number, Node_number;
    std::pair<std::pair<int,int>,double> * Nodes;
    Line3D * lines;
    // Nodes consists <line number, curve>, meaning from last line number(default 0) to this line number, the curve would be <curve> 
    Map(std::string name, int Line_number , int Node_number, std::pair<std::pair<int,int>,double> * Nodes ){
        this->name = name ,this->Line_number = Line_number, this->Nodes = Nodes, this->Node_number = Node_number;
        lines = new Line3D[Line_number];

        for(int i = 0 ; i < Line_number ; i++){
            lines[i].z = segement_length*i;
        }

        for(int i = 0 ; i < Node_number ; i++){
            for(int j = Nodes[i].first.first ; j < Nodes[i].first.second ; j++){
                lines[j].curve = Nodes[i].second;
            }
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


    ~Map(){
        delete [] Nodes;
        delete Nodes;
        delete [] lines;
        delete lines;
    }

    

};

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
    if(0){
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



/*
    std::vector< SDL_Vertex > verts =
    {
        { SDL_FPoint{ 100, 100 }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ 450, 450 }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ 300,  450}, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ 150, 150 }, SDL_Color{ 255, 255, 255, 255 }, SDL_FPoint{ 0 }, },
    };
    SDL_RenderGeometry(renderer, NULL,verts.data(), verts.size(), NULL,0 ); 
*/
    /*
    if(y1>y2){
        std::swap(y1,y2);
        std::swap(x1,x2);
        std::swap(w1,w2);
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    if(y1==y2){
        //SDL_RenderDrawLine(renderer, x1 , y1 , x2, y2 );
    }
    else{
        for(int y = y1 ; y <= y2 ; y++){
            int w = w1 + (w2 - w1) * (y - y1) / (y2 - y1) ;
            int xmid = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            SDL_RenderDrawLine(renderer, xmid - w , y, xmid + w, y);
        }        
    }
    */
}

void draw_scene(SDL_Renderer * renderer, Map * map, Camera3D * cam, int lines_drawn){
    int start_pos = cam->z / segement_length;
    int total_lines = map->Line_number;
    double x = 0, dx = 0;
    //draw_quad(renderer, SCREEN_WIDTH/2 ,0,SCREEN_WIDTH/2 , SCREEN_WIDTH/2 ,SCREEN_HEIGHT ,SCREEN_WIDTH/2, Black);
    for(int i = 1+start_pos ; i < lines_drawn+start_pos ; i++){

        //while(lines[(i+skip) % total_lines])
        
        Line3D * curr_line = &map->lines[i % total_lines];
        Line3D * prev_line = &map->lines[(i-1) % total_lines];


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


        SDL_Color grass = (i/3)%2 ? Light_Green : Dark_Green;
        SDL_Color rumble = (i/3)%2 ? White : Black;
        SDL_Color road = (i/3)%2 ? Light_Road : Dark_Road;
        SDL_Color road_mid_line = (i/3)%4 < 2 ? Light_Road : Dark_Road;

        //SDL_RenderClear(renderer);
        draw_quad(renderer, 0, prev_line->screenY, SCREEN_WIDTH, 0,curr_line->screenY, SCREEN_WIDTH, Water);
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width*1.2 , curr_line->screenX ,curr_line->screenY, curr_line->width*1.2 , rumble);
    
        draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width , curr_line->screenX ,curr_line->screenY, curr_line->width , road);

        if((i/3)%6 < 3){
            draw_quad(renderer, prev_line->screenX, prev_line->screenY, prev_line->width/32 , curr_line->screenX ,curr_line->screenY, curr_line->width/32 , White);
        }
        //draw_quad(renderer, 50,50,50,200,200,100,Light_Road);
        
        //if(i>start_pos+280){std::cout<<curr_line->width<<' '<<prev_line->width<<'\n';}
    }
    draw_quad(renderer, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, Blue_Sky); 
    cam->z+=cam->vz;
    //cam->x -= lines[1+start_pos].curve*25;
    //cam->x+=cam->vx;    
}

void draw_cars(SDL_Renderer * renderer, SDL_Texture * texture, Camera3D * cam, Car3D * car){
    texture = loadTexture(renderer, car->file_pos);
    car->project(cam);
    SDL_Rect CarViewport;
    CarViewport.w = (int) car->picture_width*car->original_scale*car->scale*1500;
    CarViewport.h = (int) car->picture_width*car->original_scale*car->scale*1500;
    CarViewport.x = car->screenX - CarViewport.w / 2;
    CarViewport.y = car->screenY - CarViewport.h / 2;

    //std::cout<<CarViewport.w<<' '<<CarViewport.h<<' '<<CarViewport.x<<' '<<CarViewport.y<<'\n';

    /*CarViewport.w = (int) 250;
    CarViewport.h = (int) 250;
    CarViewport.x = 350;
    CarViewport.y = 350;*/

    SDL_RenderSetViewport( renderer, &CarViewport );
    
    //Render texture to screen
    SDL_RenderCopy( renderer, texture, NULL, NULL );
    SDL_RenderSetViewport( renderer, NULL );
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

void framerate_cap(Uint32 start, int fps){
    static int cnt = 0;
    double ms_per_frame = 1000/fps;
    double elapsed_time = (double) (SDL_GetTicks() - start);
    if( elapsed_time < ms_per_frame ){
        SDL_Delay( (int) (ms_per_frame - elapsed_time) );
    }
    /*
    if(cnt==10){std::cout<<elapsed_time<<' ';cnt=0;}
    else{cnt++;}
    */
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
            int Node_number = 5;
            std::pair<std::pair<int,int>,double> * Nodes = new std::pair<std::pair<int,int>,double>[5];
            Nodes[0] = std::make_pair( std::make_pair(100,300) , 0.2 );
            Nodes[1] = std::make_pair( std::make_pair(500,1000) , -0.5 );
            Nodes[2] = std::make_pair( std::make_pair(1000,1200) , 1 );
            Nodes[3] = std::make_pair( std::make_pair(1400,1700) , -0.4 );
            Nodes[4] = std::make_pair( std::make_pair(1700,1800) , -1.2 );

            Map * test_map = new Map("Test_Map",5000,5,Nodes);


            //Create Camera
            Camera3D * cam = new Camera3D(0,1550,100,5,0,0);

            Car3D * AE86 = new Car3D(100,100,"test_race_img/AE86_cropped.png",1);

            //Clear renderer
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);


            int k = 0;
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
                        if(keyarr[SDL_SCANCODE_UP])
                            AE86->vz+=5;
                        if(keyarr[SDL_SCANCODE_DOWN])
                            AE86->vz-=5;
                            AE86->vz = AE86->vz >= 0 ? AE86->vz : 0;


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
                    AE86->x-=AE86->vz * 0.04 ;
                if(keyarr[SDL_SCANCODE_RIGHT])
                    AE86->x+=AE86->vz * 0.04 ;

                AE86->x-=AE86->vz * 0.01 * test_map->get_curve((int) AE86->z / segement_length );
                
        

                //Test
                SDL_SetRenderDrawColor(renderer, Blue_Sky.r,Blue_Sky.g,Blue_Sky.b,Blue_Sky.a);
                SDL_RenderClear(renderer);
                //SDL_RenderDrawLine(renderer, 0,0,100,100);
                //draw_quad(renderer, 0,100,100,500,500,200,Light_Green);
                

                //Draw Scene
                draw_scene(renderer, test_map, cam ,300);

                //draw speed
                std::ostringstream stream;
                stream << std::fixed << std::setprecision(2) << AE86->vz;
                std::string stringValue = stream.str();
                draw_words(renderer, stringValue , SCREEN_WIDTH*9/10, SCREEN_HEIGHT*9/10);
                stream.str("");
                stream.clear();

                //draw progress
                stream << std::fixed << std::setprecision(2) << AE86->z/(test_map->Line_number*segement_length)*100<<'%';
                stringValue = stream.str();
                //std::cout<<stream.str()<<'\n';
                draw_words(renderer, stringValue , SCREEN_WIDTH*1/10, SCREEN_HEIGHT*9/10);
                stream.str("");
                stream.clear();
                
                //draw cars, main car
                draw_cars(renderer, texture, cam , AE86);
                AE86->x -= test_map->lines[(int) AE86->z/segement_length].curve * AE86->vz * 0.001;
                

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

