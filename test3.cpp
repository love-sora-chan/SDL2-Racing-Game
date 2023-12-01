#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>

SDL_Window* gWindow = NULL;
SDL_Texture* gTexture = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* Font = NULL;

SDL_Color White= {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};

enum current_page{
	Home_page = 0,
	Map_Page ,
	Record_Page,
};
current_page STATUS = Home_page;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 750;

void InitializeSDL(){
	//Initialize SDL
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	
	//Initialize SDL_image
	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP | IMG_INIT_JXL | IMG_INIT_AVIF;
	if( !( IMG_Init( imgFlags ) & imgFlags ))
	{
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
	}
	
	//Initialize SDL_ttf
	if( TTF_Init() == -1 )
	{
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	//Initialize SDL_mixer
	int mixFlags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_OPUS | MIX_INIT_FLAC;
	if( !( Mix_Init( mixFlags ) & mixFlags ))
	{
		printf("SDL_mix could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}
	//open audio devices
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
	    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
	}
	
	
}

class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();
		//Deallocates texture
		void free();

		//Loads image at the specified path
		bool loadFromFile( std::string path , SDL_Renderer*);
		
		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		//Set blending
		void setBlendMode( SDL_BlendMode blending );
		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at the given point
		void render(float x, float y, SDL_Renderer*);
		void render(float x, float y, float w, float h, SDL_Renderer*);
		void render(SDL_FRect& dstrect, SDL_Rect& srcrect, SDL_Renderer*);

		//Explained in Unit 5.
		//Load Texture from text
		bool loadFromRenderedText( std::string text, SDL_Color, SDL_Renderer*, TTF_Font*);
		//Render center-aligned text
		void renderCenter(float x, float y, SDL_Renderer*);
		void renderRight(float x, float y, SDL_Renderer*);
		
		//Gets image dimensions
		float getWidth();
		float getHeight();
		//set mHeight and mWidth
		void setdim(float w, float h);
		
	protected:
		//The actual hardware texture
		SDL_Texture* mTexture;
		//Image dimensions
		float mWidth;
		float mHeight;
};

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
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
LTexture::~LTexture(){
	free();
}

bool LTexture::loadFromFile( std::string path, SDL_Renderer* renderer)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at the specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		/*
		//Color key image: Optional
		//Further explanation in Unit ?
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
		*/

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
	//Optional
	this->setBlendMode( SDL_BLENDMODE_BLEND );
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText( std::string text, SDL_Color textColor, SDL_Renderer* renderer, TTF_Font* font)
{
    //Get rid of preexisting texture
    free();

    //check if the font is loaded
    if( font == NULL )
    {
        printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
    }

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, text.c_str() , textColor );
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

    this->setBlendMode( SDL_BLENDMODE_BLEND );
    //Return success
    return mTexture != NULL;
}

void LTexture::render( float x, float y, SDL_Renderer* renderer){
	//Set rendering space and render to screen
	SDL_FRect target = { x, y, mWidth, mHeight };
	//Render to screen
	SDL_RenderCopyF( renderer, mTexture, NULL, &target);
}
void LTexture::render( float x, float y, float w, float h, SDL_Renderer* renderer){
	//Set rendering space and render to screen
	SDL_FRect target = { x, y, w, h };
	//Render to screen
	SDL_RenderCopyF( renderer, mTexture, NULL, &target);
}
void LTexture::render( SDL_FRect& dstrect, SDL_Rect& srcrect, SDL_Renderer* renderer){
	//Render to screen
	SDL_RenderCopyF( renderer, mTexture, &srcrect, &dstrect);
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

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
		void show(Record &rec,SDL_Renderer* REND);

		
		
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

void Record::show(Record &rec,SDL_Renderer* REND){
	SDL_RenderClear(REND);
	Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",14);
	for(int i=0;i<5;i++){
		rec.ranking_list->loadFromRenderedText("RANK"+std::to_string(rec.ranking[i]),White,REND,Font);
		rec.ranking_list->render(0,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH/4,SCREEN_HEIGHT/7,REND);
		rec.time_list->loadFromRenderedText(std::to_string(rec.complete_time[i]),White,REND,Font);
		rec.time_list->render(SCREEN_WIDTH/2,SCREEN_HEIGHT/7*(i+2),SCREEN_WIDTH/2,SCREEN_HEIGHT/7,REND);
		
	}
	SDL_RenderPresent(REND);
}


class MENU{
	private:
		int options=2;
		LTexture background,start_button,record_button;
	
	public:
		int opt=1;
		//initialize
		void Initialize(MENU &m,SDL_Renderer* REND);
		
		//show
		void show(MENU &m,SDL_Renderer* REND);

		//if going up or down
		void UP_opt(MENU &m);
		void DOWN_opt(MENU &m);

		//which button
		void choosing(int opt,MENU &m,SDL_Renderer* REND);

		//appear or vanish in transition
		void vanish(MENU &m,SDL_Renderer* REND,int spd );
		void appear(MENU &m,SDL_Renderer* REND,int spd );

		
};
void MENU::Initialize(MENU &m,SDL_Renderer* REND){
	m.opt = 1;m.options = 2;
	m.background.loadFromFile("testimage/black.png",REND);
}

void MENU::show(MENU &m,SDL_Renderer* REND){
	SDL_RenderClear(REND);
	m.background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,REND);
	m.start_button.render(SCREEN_WIDTH*0.375,SCREEN_HEIGHT*0.45,SCREEN_WIDTH/4,SCREEN_HEIGHT/10,REND);
	m.record_button.render(SCREEN_WIDTH*0.375,SCREEN_HEIGHT*0.60,SCREEN_WIDTH/4,SCREEN_HEIGHT/10,REND);
	SDL_RenderPresent(REND);
}

void MENU::DOWN_opt(MENU &m){
	if(m.opt<m.options)	m.opt++;
	else m.opt = m.options;
}
void MENU::UP_opt(MENU &m){
	if(m.opt>0)	opt--;
	else m.opt = 1;
}

void MENU::choosing(int opt,MENU &m,SDL_Renderer* REND){
	switch(opt){
		case 1:{
			m.start_button.loadFromFile("testimage/start_chosen.png",REND);
			m.record_button.loadFromFile("testimage/record_unchosen.png",REND);
			break;
		}
		case 2:{
			m.start_button.loadFromFile("testimage/start_unchosen.png",REND);
			m.record_button.loadFromFile("testimage/record_chosen.png",REND);
			break;
		}
	}
}



void MENU::appear(MENU &m,SDL_Renderer* REND,int spd = 5){
	for(int i=0;i<256;i+=spd){
		SDL_Delay(1);
		SDL_RenderClear(REND);
		m.start_button.setAlpha(i);
		m.record_button.setAlpha(i);
		m.show(m,REND);
	}
	
}

void MENU::vanish(MENU &m,SDL_Renderer* REND,int spd = 5){
	for(int i=255;i>=0;i-=spd){
		SDL_Delay(1);
		SDL_RenderClear(REND);
		m.start_button.setAlpha(i);
		m.record_button.setAlpha(i);
		m.show(m,REND);
	}
	
}

void Transition(SDL_Renderer* REND){
	SDL_RenderClear(REND);
	LTexture load_pic;
	load_pic.loadFromFile("testimage/loading.png",REND);
	load_pic.render(SCREEN_WIDTH/5*2,SCREEN_HEIGHT/2,SCREEN_WIDTH/5,SCREEN_HEIGHT/4,REND);
	SDL_RenderPresent(REND);										
	SDL_Delay(1000);
}




int WinMain(int argc,char *argv[]){
	InitializeSDL();
	gWindow = SDL_CreateWindow( "TESTING", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	//You'd better check if it success!
	if(gWindow == NULL){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError() );
	}else{
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(gRenderer == NULL){
			printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else{
			
			Record rec;
			rec.RecordReset(rec);

			MENU main_menu;
			main_menu.Initialize(main_menu,gRenderer);
			main_menu.choosing(main_menu.opt,main_menu,gRenderer);
			main_menu.show(main_menu,gRenderer);


			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					
					else if(e.type == SDL_KEYDOWN){
						switch((int)STATUS){
							case 0:{
								switch(e.key.keysym.sym){
									case SDLK_UP:{
										SDL_RenderClear(gRenderer);
										main_menu.UP_opt(main_menu);
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.show(main_menu,gRenderer);
										break;
									}
									case SDLK_DOWN:{
										SDL_RenderClear(gRenderer);
										main_menu.DOWN_opt(main_menu);
										main_menu.choosing(main_menu.opt,main_menu,gRenderer);
										main_menu.show(main_menu,gRenderer);
										break;
									}
									case SDLK_RETURN:{
										SDL_RenderClear(gRenderer);
										if(main_menu.opt==2){
											STATUS = Record_Page;
											Transition(gRenderer);
											rec.show(rec,gRenderer);											
										}
										break;
									}
									default:
										STATUS = Home_page;break;
									}
								break;
							}
							case 2:{
								if(e.key.keysym.sym==SDLK_b){
									STATUS = Home_page;
									Transition(gRenderer);
									main_menu.Initialize(main_menu,gRenderer);
									main_menu.choosing(main_menu.opt,main_menu,gRenderer);
									main_menu.appear(main_menu,gRenderer);
									break;								
								}									
							}
						}
						
					}
				}
				SDL_UpdateWindowSurface(gWindow);
		}
		SDL_DestroyRenderer(gRenderer);
	}
	SDL_DestroyWindow(gWindow);
	return 0;
	}
}