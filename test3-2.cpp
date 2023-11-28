#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

SDL_Window* gWindow = NULL;
SDL_Texture* gTexture = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* Font = NULL;

SDL_Color White= {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};

enum current_page{
	Home_page = 0,
	Map_Page,
	Record_Page,
};

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
	Font = TTF_OpenFont("test_ttf/SegUIVar.ttf",14);
	for(int i=0;i<5;i++){
		rec.ranking_list->loadFromRenderedText(std::to_string(rec.ranking[i]),White,REND,Font);
		rec.ranking_list->render(0,SCREEN_HEIGHT/5*i,SCREEN_WIDTH/16,SCREEN_HEIGHT/5,REND);
		rec.time_list->loadFromRenderedText(std::to_string(rec.complete_time[i]),White,REND,Font);
		rec.time_list->render(SCREEN_WIDTH/2,SCREEN_HEIGHT/5*i,SCREEN_WIDTH/2,SCREEN_HEIGHT/5,REND);
		SDL_RenderPresent(REND);
	}
}





void appear(LTexture &TEXT,SDL_Renderer* REND){
	for(int i=0;i<256;i+=5){
		SDL_Delay(1);
		SDL_RenderClear(REND);
		TEXT.setAlpha(i);
		TEXT.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,gRenderer);
		SDL_RenderPresent(REND);
	}
	
}

void vanish(LTexture &TEXT,SDL_Renderer* REND){
	for(int i=255;i>=0;i-=5){
		SDL_Delay(1);
		SDL_RenderClear(REND);
		TEXT.setAlpha(i);
		TEXT.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,gRenderer);
		SDL_RenderPresent(REND);
	}
	
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
			current_page STATUS;
			Record rec;
			rec.RecordReset(rec);

			LTexture text_background;
			text_background.loadFromFile("testimage/black.png",gRenderer);
			text_background.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,gRenderer);
			
			LTexture text1;
			text1.loadFromFile("testimage/bg.png",gRenderer);
			text1.setAlpha(0);
			text1.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,gRenderer);
			SDL_RenderPresent(gRenderer);

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
						switch(e.key.keysym.sym){
							case SDLK_v:
								vanish(text1,gRenderer);break;

							case SDLK_a:
								appear(text1,gRenderer);break;

							case SDLK_s:{
								LTexture load_pic;
								load_pic.loadFromFile("testimage/loading.png",gRenderer);
								load_pic.render(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,gRenderer);
								SDL_RenderPresent(gRenderer);
								SDL_Delay(1000);
								SDL_RenderClear(gRenderer);
								rec.show(rec,gRenderer);break;}

							case SDLK_ESCAPE:
								SDL_RenderClear(gRenderer);
								SDL_RenderPresent(gRenderer);break;
						}
					}
				}
		}
		SDL_DestroyRenderer(gRenderer);
	}
	SDL_DestroyWindow(gWindow);
	return 0;
	}
}