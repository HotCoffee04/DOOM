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
//	System interface for sound.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_unix.c,v 1.5 1997/02/03 22:45:10 b1 Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>

#include <sys/time.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Timer stuff. Experimental.
#include <time.h>
#include <signal.h>

#include <errno.h>

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"

#include "doomdef.h"

#define CHUNK_BUFFER_SIZE 8 //number of sounds that can be played at the same time AKA number of channels

// Get the interrupt. Set duration in millisecs.
int I_SoundSetTimer( int duration_of_tick );
void I_SoundDelTimer( void );


typedef struct chunk_n{ //chunk node

  int id;
  int sfxid;
  Mix_Chunk *chunk;
  struct chunk_n *next;

}chunk_n;


chunk_n sounds[CHUNK_BUFFER_SIZE]; //chunk circular buffer
Mix_Music *music;
SDL_RWops *rwops;
chunk_n *cpoint;

#define SPECIAL_SOUNDS_NUMBER 6
int specialSounds[] = {sfx_sawup,sfx_sawidl,sfx_sawful,sfx_sawhit,sfx_stnmov,sfx_pistol}; //sounds that need to be played one at a a time


void I_SetChannels()
{

}	

 
void I_SetSfxVolume(int volume)
{

}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}

void I_SetMusicVolume(int volume)
{

}


int
I_StartSound
( int		id,
  int		vol,
  int		sep,
  int		pitch,
  int		priority )
{
  //printf("I_StartSound(%d)\n",id);

  int special = 0;
  //check if special sound
  for(int i = 0; i < SPECIAL_SOUNDS_NUMBER; i++){
    if(specialSounds[i] == id)
      special = 1;
  }

  //check if sound is already playing
  //if(special)
  for(int i = 0; i < CHUNK_BUFFER_SIZE; i++){
    if(sounds[i].sfxid == id){
      Mix_HaltChannel(sounds[i].id);  //stop the special sound
    } 
  }

  //allocates new sound
  cpoint->chunk = malloc(sizeof(Mix_Chunk));
  cpoint->chunk->allocated = 1;
  cpoint->chunk->volume = vol * 16 - 1;

  if(id == sfx_sawful || id == sfx_sawidl){ //if it's the saw sound we just load a forth of the data for some reasons beyond my understanding

    cpoint->chunk->alen = W_LumpLength(S_sfx[id].lumpnum) / 4;
    cpoint->chunk->abuf = (W_CacheLumpNum(S_sfx[id].lumpnum,PU_CACHE) + 32); 


  }else{
    cpoint->chunk->abuf = (W_CacheLumpNum(S_sfx[id].lumpnum,PU_CACHE) + 32); 
    cpoint->chunk->alen = W_LumpLength(S_sfx[id].lumpnum) - 32;
  }

  cpoint->sfxid = id;




  if(Mix_PlayChannel(cpoint->id,cpoint->chunk,0) == -1){
    printf("Sound Error %s\n",Mix_GetError());
  }

  cpoint = cpoint->next;
  return cpoint->id;
}



void I_StopSound (int handle)
{
  //printf("I_StopSound(%d)\n",handle);


  Mix_HaltChannel(handle);
}


int I_SoundIsPlaying(int handle)
{
    // Ouch.
    return gametic < handle;
}




//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the global
//  mixbuffer, clamping it to the allowed range,
//  and sets up everything for transferring the
//  contents of the mixbuffer to the (two)
//  hardware channels (left and right, that is).
//
// This function currently supports only 16bit.
//
void I_UpdateSound( void )
{
}


// 
// This would be used to write out the mixbuffer
//  during each game loop update.
// Updates sound buffer and audio device at runtime. 
// It is called during Timer interrupt with SNDINTR.
// Mixing now done synchronous, and
//  only output be done asynchronous?
//
void
I_SubmitSound(void)
{

}



void
I_UpdateSoundParams
( int	handle,
  int	vol,
  int	sep,
  int	pitch)
{
  // UNUSED.
  handle = vol = sep = pitch = 0;
}




void I_ShutdownSound(void)
{    
  //printf("I_ShutdownSound()\n");

  //free buffer
  for(int i = 0; i < CHUNK_BUFFER_SIZE; i++){
    if(sounds[i].chunk->allocated){
      //Mix_FreeChunk(sounds[i].chunk); for some reasons its considered an "invalid pointer"
    }
  }

  Mix_CloseAudio();
  SDL_Quit();
  return;
}



void
I_InitSound()
{ 
  //printf("I_InitSound()\n");
  SDL_Init(SDL_INIT_AUDIO);
  Mix_OpenAudio(11025,AUDIO_U8,1,64);

  //inizialize sound buffer
  for(int i = 0; i < CHUNK_BUFFER_SIZE - 1; i++){
    sounds[i].id = i;
    sounds[i].next = &sounds[i+1];
  }
  sounds[CHUNK_BUFFER_SIZE - 1].next = &sounds[0];
  sounds[CHUNK_BUFFER_SIZE - 1].id = CHUNK_BUFFER_SIZE - 1;
  cpoint = &sounds[0];
}




//
// MUSIC API.
// Still no music done.
// Remains. Dummies.
//
void I_InitMusic(void)		{ }
void I_ShutdownMusic(void)	{ }



void I_PlaySong(int handle, int looping)
{
  // UNUSED.
  //if(Mix_PlayChannel(-1,music,-1) == -1)
  //printf("Music Error %s\n",Mix_GetError());
}

void I_PauseSong (int handle)
{
  //printf("I_PauseSong()\n");
  // UNUSED.

  //Mix_PlayMusic(music,-1);

  handle = 0;
}

void I_ResumeSong (int handle)
{
  //printf("I_ResumeSong()\n");
  // UNUSED.
  handle = 0;
}

void I_StopSong(int handle)
{
  //printf("I_StopSong()\n");
  // UNUSED.
  handle = 0;
}

void I_UnRegisterSong(int handle)
{
  //printf("I_UnRegisterSong()\n");

  handle = 0;
}

int I_RegisterSong(void* data)
{

  return 1;
}

// Is the song playing?
int I_QrySongPlaying(int handle)
{
  // UNUSED.
  handle = 0;
  return 1;
}
