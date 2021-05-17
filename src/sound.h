
#define SOUND_UGH		0
#define SOUND_BOING		1
#define SOUND_BONK		2
#define SOUND_BURP		3
#define SOUND_LASER		4
#define SOUND_OW		5
#define SOUND_WEE		6
#define SOUND_EXPLOSION         7
#define SOUND_BZZT              8
#define SOUND_BEEP              9
#define SOUND_SHOOMF           10
 
class SoundSystem
{
  char current_track [ 256 ] ;
#ifdef USE_SDL_MIXER
  Mix_Music * music ;
#else
  slScheduler *sched ;
#endif

public:
  SoundSystem () ;
  ~SoundSystem() ;

  void update () ;
  void playSfx ( int sound ) ;

  void setSafetyMargin ( float t = 0.25 )
  {
#ifndef USE_SDL_MIXER
    sched -> setSafetyMargin ( t ) ;
#endif
  }

  void  change_track ( char *fname ) ;
  void    stop_music () ;
  void   start_music () ;

  void disable_music () ;
  void  enable_music () ;

  void disable_sfx   () ;
  void  enable_sfx   () ;
} ;

