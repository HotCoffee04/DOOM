// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff, SDL
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include <stdarg.h>
#include <sys/types.h>


#include <errno.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *surface;
SDL_Event sdlEvent;
SDL_Texture *texture;
SDL_Palette *sdlPalette;


int lastMouseX,lastMouseY;

float forcedRatio;
float ratio;
float winScale;
int yoffset;
int xoffset;
SDL_Rect imgRect;

void resizedWindow(){

	int nx,ny;
	SDL_GetWindowSize(win,&nx,&ny);
	int sx,sy;

	ratio = (float)nx / ny;

	xoffset = yoffset = 0;

	if(ratio > forcedRatio){

		winScale = (float)ny/SCREENHEIGHT;

		sx = (SCREENHEIGHT*forcedRatio) * winScale;
		sy = SCREENHEIGHT * winScale;


		xoffset = nx / 2 - ( sx/2);
	}else{

		winScale = (float)nx / (float)SCREENWIDTH;

		sx = SCREENWIDTH * winScale;
		sy = (SCREENWIDTH / forcedRatio) * winScale;


		yoffset = ny / 2 - ( sy/2);
	}

	imgRect.x = xoffset;
	imgRect.y = yoffset;
	imgRect.h = sy;
	imgRect.w = sx;
}

int SdlKeyToDoomKey(SDL_Keysym key){

	switch(key.scancode){

		case SDL_SCANCODE_RETURN: return KEY_ENTER;
		case SDL_SCANCODE_ESCAPE: return KEY_ESCAPE;

		case SDL_SCANCODE_UP: return KEY_UPARROW;
		case SDL_SCANCODE_DOWN: return KEY_DOWNARROW;
		case SDL_SCANCODE_LEFT: return KEY_LEFTARROW;
		case SDL_SCANCODE_RIGHT: return KEY_RIGHTARROW;
		
		case SDL_SCANCODE_F1: return KEY_F1;
		case SDL_SCANCODE_F2: return KEY_F2;
		case SDL_SCANCODE_F3: return KEY_F3;
		case SDL_SCANCODE_F4: return KEY_F4;
		case SDL_SCANCODE_F5: return KEY_F5;
		case SDL_SCANCODE_F6: return KEY_F6;
		case SDL_SCANCODE_F7: return KEY_F7;
		case SDL_SCANCODE_F8: return KEY_F8;
		case SDL_SCANCODE_F9: return KEY_F9;
		case SDL_SCANCODE_F10: return KEY_F10;
		case SDL_SCANCODE_F11: return KEY_F11;
		case SDL_SCANCODE_F12: return KEY_F12;

		case SDL_SCANCODE_LSHIFT:
		case SDL_SCANCODE_RSHIFT: return KEY_RSHIFT;
		case SDL_SCANCODE_LCTRL:
		case SDL_SCANCODE_RCTRL: return KEY_RCTRL;
		case SDL_SCANCODE_LALT:
		case SDL_SCANCODE_RALT: return KEY_RALT;

		case SDL_SCANCODE_PAUSE: return KEY_PAUSE;
		case SDL_SCANCODE_BACKSPACE: return KEY_BACKSPACE;

		case SDL_SCANCODE_MINUS: return KEY_MINUS;
		case SDL_SCANCODE_EQUALS: return KEY_EQUALS;

		case SDL_SCANCODE_SPACE: return ' ';
	
		case SDL_SCANCODE_0: return '0';

		case SDL_SCANCODE_COMMA: return ',';
		case SDL_SCANCODE_PERIOD: return '.';

		default:
		if(key.scancode >= SDL_SCANCODE_A && key.scancode <= SDL_SCANCODE_Z)
			return 'a' + (key.scancode - SDL_SCANCODE_A); 

		if(key.scancode >= SDL_SCANCODE_1 && key.scancode <= SDL_SCANCODE_0)
			return '1' + (key.scancode - SDL_SCANCODE_1); 

			return -1;
		break;
	}

}

void I_InitGraphics(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("Coffee Doom",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREENWIDTH*3, SCREENHEIGHT*3, SDL_WINDOW_RESIZABLE);
	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	//texture = SDL_CreateTexture(rend,SDL_PIXELFORMAT_RGB332,SDL_TEXTUREACCESS_STREAMING,SCREENWIDTH,SCREENHEIGHT);
	surface = SDL_CreateRGBSurface(0,SCREENWIDTH,SCREENHEIGHT,8,0,0,0,0);



	//runs doom with a stretched 4:3 aspect ratio like the one monitors had in the 90s
    if (M_CheckParm("-stretch"))
		forcedRatio = 1.33333f; //4:3
	else
		forcedRatio = 1.6f; //16:10


    if (M_CheckParm("-fullscreen")){

		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);

		SDL_SetWindowSize(win,&DM.w,&DM.h);

		SDL_SetWindowFullscreen(win,SDL_WINDOW_FULLSCREEN_DESKTOP);

	}else{
		//SDL_MaximizeWindow(win);
	}
	SDL_WarpMouseInWindow(win,100,100);
	resizedWindow();
	SDL_SetWindowGrab(win,SDL_TRUE);
	SDL_ShowCursor(1);
	SDL_SetRelativeMouseMode(1);

}


void I_ShutdownGraphics(void)
{
	SDL_FreeSurface(surface);
	SDL_ShowCursor(1);
	SDL_Quit();
}



//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?

}


//
// I_StartTic
//
void I_StartTic (void)
{

	event_t doomEvent;

	//computes events
	while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
 
        case SDL_QUIT:
			I_Quit();
        break;
 
        case SDL_KEYDOWN:

			doomEvent.type = ev_keydown;
			doomEvent.data1 = SdlKeyToDoomKey(sdlEvent.key.keysym);
			doomEvent.data2 = doomEvent.data3 = 0;
			if(doomEvent.data1 != -1)
			D_PostEvent(&doomEvent);
		break;
        case SDL_KEYUP:
			doomEvent.type = ev_keyup;
			doomEvent.data1 = SdlKeyToDoomKey(sdlEvent.key.keysym);
			doomEvent.data2 = doomEvent.data3 = 0;
			if(doomEvent.data1 != -1)
			D_PostEvent(&doomEvent);
		break;
		case SDL_MOUSEBUTTONDOWN:
			doomEvent.type = ev_keydown;
			doomEvent.data1 = KEY_RCTRL;
			doomEvent.data2 = doomEvent.data3 = 0;
			D_PostEvent(&doomEvent);

		break;
		case SDL_MOUSEBUTTONUP:
			doomEvent.type = ev_keyup;
			doomEvent.data1 = KEY_RCTRL;
			doomEvent.data2 = doomEvent.data3 = 0;
			D_PostEvent(&doomEvent);

		break;
		case SDL_MOUSEMOTION:
			doomEvent.type = ev_mouse;

			doomEvent.data2 = sdlEvent.motion.xrel * 30;
			doomEvent.data3 = sdlEvent.motion.yrel * 30;
			doomEvent.data1 = 0;


			//wrap mouse around window
			int sx,sy,cx,cy;
			int move = 0;
			SDL_GetWindowSize(win,&sx,&sy);
			SDL_GetMouseState(&cx,&cy);

			if(cx > sx - 2){
				cx = 1;	move = 1;
			}
			if(cx <= 0){
				cx = sx - 3; move = 1;
			}
			if(cy > sy - 2){
				cy = 1; move = 1;
			}
			if(cy <= 0){
				cy = sy - 3; move = 1;
			}
			//printf("%d %d (%d %d)\n",cx,cy,sx,sy);
			if(move){
				SDL_WarpMouseInWindow(win,cx,cy);

			}

				D_PostEvent(&doomEvent);

		break;
		//resize sdl window
		case SDL_WINDOWEVENT:
			switch(sdlEvent.window.event){
				case SDL_WINDOWEVENT_RESIZED:
					resizedWindow();
				break;
			}
			break;
        }
    }

	

	
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{
	SDL_RenderClear(rend);

	for(int i = 0; i < SCREENHEIGHT*SCREENWIDTH;i++)
	*((byte *)surface->pixels + i) = *(screens[0] + i);


	texture = SDL_CreateTextureFromSurface(rend,surface);

	SDL_RenderCopy(rend,texture,0,&imgRect);

	SDL_DestroyTexture(texture);


	SDL_RenderPresent(rend);

	SDL_Delay(1000 / 35);

}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{

	SDL_FreePalette(sdlPalette);
	sdlPalette = SDL_AllocPalette(256);

	byte *p = palette;

	for(int i = 0; i <= 255; i++,p+=3){

		sdlPalette->colors[i].r = *p;
		sdlPalette->colors[i].g = *(p + 1);
		sdlPalette->colors[i].b = *(p + 2);
	}

	surface->format->palette = sdlPalette;

}


