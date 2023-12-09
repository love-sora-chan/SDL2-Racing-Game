#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>

#ifndef LTexture_cpp
#define LTexture_cpp

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
		int getWidth();
		int getHeight();
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

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}



#endif