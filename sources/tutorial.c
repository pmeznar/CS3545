#include "headers/SDL/SDL.h"
#include "headers/SDL/SDL_main.h"
#include "headers/SDL/SDL_opengl.h"
#include <stdio.h>

static int user_exit = 0;

int SDL_main(int argc, char* argv[])
{
	SDL_Event		event;
	SDL_Surface		*screen;

	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
	{
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_WM_SetCaption("SDL Window", "SDL Window");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Initialize window
	screen = SDL_SetVideoMode(1024, 768, 32, SDL_OPENGL);
	if(!screen)
	{
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	//Main loop
	while(!user_exit)
	{
		//Handle input
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_MOUSEMOTION:
				break;
			case SDL_QUIT:
				exit(0);
			}
		}
	}

	SDL_Quit();

	return 0;
}
