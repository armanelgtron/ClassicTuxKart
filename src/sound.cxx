
#include "tuxkart.h"

struct Sound
{
   const char *fname ;
#ifdef USE_SDL_MIXER
   Mix_Chunk * s ;
#else
   slSample *s ;
#endif
} ;


static Sound sfx [] =
{
  { "wavs/ugh.wav"	, NULL },
  { "wavs/boing.wav"	, NULL },
  { "wavs/bonk.wav"	, NULL },
  { "wavs/burp.wav"	, NULL },
  { "wavs/laser.wav"	, NULL },
  { "wavs/ow.wav"	, NULL },
  { "wavs/wee.wav"	, NULL },
  { "wavs/explosion.wav", NULL },
  { "wavs/bzzt.wav"	, NULL },
  { "wavs/horn.wav"	, NULL },
  { "wavs/shoomf.wav"	, NULL },
  { NULL, NULL }
} ;

static int music_off = FALSE ;
static int   sfx_off = FALSE ;


void SoundSystem::stop_music ()
{
#ifdef USE_SDL_MIXER
  //Mix_HaltMusic () ;
  Mix_FadeOutMusic ( 250 ) ;
#else
  sched -> stopMusic () ;
  sched -> update    () ;  /* Ugh! Nasty Kludge! */
  sched -> update    () ;  /* Ugh! Nasty Kludge! */
#endif
}


void SoundSystem::change_track ( char *fname )
{
  stop_music () ;

  if ( fname == NULL )
    fname = "" ;

  if ( strcmp ( fname, current_track ) != 0  )
  {
    strcpy ( current_track, fname ) ;
  }
  
  start_music () ;
}

void SoundSystem::start_music ()
{
#ifdef USE_SDL_MIXER
  if ( Mix_PlayingMusic() )
  {
    Mix_HaltMusic () ;
  }
  
  if ( music )
  {
    Mix_FreeMusic ( music ) ;
  }
  
  music = Mix_LoadMUS ( current_track );
  if ( !music )
  {
    fprintf( stderr, "Failed to load music: %s\n", Mix_GetError() ) ;
  }
  else
  {
    Mix_PlayMusic ( music, -1 ) ;
    Mix_VolumeMusic ( MIX_MAX_VOLUME ) ;
  }
#else
  sched -> stopMusic () ;

  if ( current_track [ 0 ] != '\0' )
    sched -> loopMusic ( current_track ) ;
#endif
}


void SoundSystem::disable_music () {  stop_music () ; music_off = TRUE  ; }
void SoundSystem:: enable_music () { start_music () ; music_off = FALSE ; }

void SoundSystem::disable_sfx () { sfx_off = TRUE  ; }
void SoundSystem:: enable_sfx () { sfx_off = FALSE ; }



void SoundSystem::playSfx ( int sfx_num )
{
  if ( ! sfx_off )
  #ifdef USE_SDL_MIXER
    if ( Mix_PlayChannel ( sfx_num, sfx[sfx_num].s, 0 ) == -1 )
    {
      printf("Mix_PlayChannel: %s\n",Mix_GetError());
    }
  #else
    sched -> playSample ( sfx[sfx_num].s, 1, SL_SAMPLE_MUTE, 2, NULL ) ;
  #endif
}


SoundSystem::SoundSystem ()
{
#ifdef USE_SDL_MIXER
  music = NULL;
  Mix_Init ( MIX_INIT_MOD ) ;
  if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 )
  {
    fprintf( stderr, "Failed to open audio: %s\n", Mix_GetError() ) ;
    sfx_off = music_off = TRUE ;
    
    /* safety */
    for ( Sound *currsfx = &(sfx[0]) ; currsfx -> fname != NULL ; currsfx++ )
    {
      currsfx -> s  = NULL ;
    }
  }
  else
  {
    /* For now, each sound will play on its own channel... */
    Mix_AllocateChannels(12);
    for ( Sound *currsfx = &(sfx[0]) ; currsfx -> fname != NULL ; currsfx++ )
    {
      currsfx -> s  = Mix_LoadWAV ( currsfx -> fname ) ;
      if ( ! currsfx -> s )
      {
        fprintf( stderr, "Failed to load %s: Mix_LoadWAV: %s\n", currsfx -> fname, Mix_GetError() );
      }
      else
      {
        Mix_VolumeChunk ( currsfx -> s, MIX_MAX_VOLUME/4 ) ;
      }
    }
  }
#else
  sched = new slScheduler ;

  setSafetyMargin () ;

  for ( Sound *currsfx = &(sfx[0]) ; currsfx -> fname != NULL ; currsfx++ )
    currsfx -> s  = new slSample ( currsfx -> fname, sched ) ;

  enable_sfx   () ;
  change_track ( "" ) ;
  enable_music () ;
#endif
}


SoundSystem::~SoundSystem ()
{
#ifdef USE_SDL_MIXER
  Mix_AllocateChannels(0);
  
  for ( Sound *currsfx = &(sfx[0]) ; currsfx -> fname != NULL ; currsfx++ )
    Mix_FreeChunk ( currsfx -> s ) ;

  Mix_CloseAudio() ;
  Mix_Quit() ;
#else
  sched -> stopMusic () ;
  
  for ( Sound *currsfx = &(sfx[0]) ; currsfx -> fname != NULL ; currsfx++ )
    delete currsfx -> s;
  
  delete sched ;
#endif
}

void SoundSystem::update ()
{
#ifndef USE_SDL_MIXER
  /*
    Comment this next line out if the
    sound causes big glitches on your
    IRIX machine!
  */

  sched -> update () ;
#endif
}


