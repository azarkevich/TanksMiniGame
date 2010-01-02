// Tanks-SDL.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include "World.h"

SDL_Surface* load_image(const char *file)
{
	SDL_Surface *temp1, *temp2;
	temp1 = SDL_LoadBMP(file);
	temp2 = SDL_DisplayFormat(temp1);
	SDL_FreeSurface(temp1);

	return temp2;
}

vector<SDL_Surface*> load_sprites(const char *file,
		int width, int height,
		int border_width, int border_height,
		int color_key_r, int color_key_g, int color_key_b)
{
	vector<SDL_Surface*> sprites;
	SDL_Surface* multi = SDL_LoadBMP(file);
	if(multi == NULL)
	{
		fprintf(stderr, "Couldn't load sprites from: %s, error: %s\n", file, SDL_GetError());
		exit(1);
	}

	SDL_Surface *tmp = SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCCOLORKEY,
		width,
		height,
		32,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000);
	if(tmp == NULL)
	{
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		exit(1);
	}

	int cols =  (multi->w - border_width) / (width + border_width);
	int rows =  (multi->h - border_height) / (height + border_height);

	for(int row=0;row<rows;row++)
	{
		for(int col=0;col<cols;col++)
		{
			SDL_Rect srcr;
			srcr.x = border_width + (width + border_width)*col;
			srcr.y = border_height + (height + border_height)*row;
			srcr.w = width;
			srcr.h = height;

			SDL_BlitSurface(multi, &srcr, tmp, NULL);

			SDL_SetColorKey(tmp, SDL_SRCCOLORKEY, SDL_MapRGB(tmp->format, color_key_r, color_key_g, color_key_b));

			sprites.push_back(SDL_DisplayFormat(tmp));
		}
	}

	return sprites;
}

void draw_image(SDL_Surface *s, SDL_Surface *img, int x, int y)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	SDL_BlitSurface(img, NULL, s, &dest);
}

int main(int argc, char **argv)
{
	/* Initialize the SDL library */
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0 )
	{
        	fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        	exit(1);
	}

	/* Clean up on exit */
	atexit(SDL_Quit);

	SDL_Surface *screen = SDL_SetVideoMode(800, 600, 0, SDL_HWSURFACE | SDL_DOUBLEBUF /* | SDL_FULLSCREEN*/);
	if (screen == NULL)
	{
        	fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        	exit(1);
	}

	SDL_WM_SetCaption("Tanks", "t");

	World w(screen->w, screen->h);
	w.tiles = load_sprites("resources/sprites.bmp", 32, 32, 1, 1, 0, 0, 0);
	w.load_level("\
wwwwwwwwwwwwwwwwwwww\n\
w                  w\n\
w    ww wwww       w\n\
w          w       w\n\
w       b  wwww    w\n\
w             www  w\n\
w         b   www  w\n\
w             www  w\n\
w      bb     www  w\n\
w      bb          w\n\
w                  w\n\
w  b   b           w\n\
w                  w\n\
wt                 w\n\
wwwwwwwwwwwwwwwwwwww");
	
	SDL_Event ev;

	int delay = 0;
	if(argc > 1)
	{
		delay = atoi(argv[1]);
	}

	while(true)
	{
		while(SDL_PollEvent(&ev))
		{
			if((ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) || ev.type == SDL_QUIT)
			{
				exit(0);
			}

			if(ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP)
			{
				w.handle_input(ev);
			}
		}

		w.think();

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 200, 200, 200));

		w.draw(screen);

		SDL_Flip(screen);

		if(delay > 0)
			SDL_Delay(delay);
	}

	return 0;
}
