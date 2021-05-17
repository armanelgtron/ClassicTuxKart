
//#include "start_tuxkart.h"
#include "tuxkart.h"
#include <plib/pu.h>
#include <signal.h>

int width=800,height=600,fullscreen=0;
extern int player;

/***********************************\
*                                   *
* These are the PUI widget pointers *
*                                   *
\***********************************/

static int numAIs = 3 ;
static int numLaps = 5 ;

static char * hostname = "";

static char        numLapsLegend [ 100 ] ;
static char        numAIsLegend  [ 100 ] ;
static char        *datadir = 0 ;

//static puButton    *cheaterAIs    ;
static puInput     *hostnameText  ;
static puSlider    *numAIsSlider  ;
static puButton    *numAIsText    ;
static puSlider    *numLapsSlider ;
static puButton    *numLapsText   ;
static puButton    *playButton    ;
static puButton    *exitButton    ;
static puButtonBox *trackButtons  ;
static fntTexFont  *fnt_sorority  ;
static puFont      *sorority      ;
static fntTexFont  *fnt_avantgarde;
static puFont      *avantgarde    ;

static ssgSimpleState *intro_gst ;

#define MAX_TRACKS 10

static puButton    *pleaseWaitButton    ;
static int startup_counter = 0 ;

static char *track_names  [ MAX_TRACKS ] ;
static char *track_idents [ MAX_TRACKS ] ;

extern int tuxkart_main ( int nl, char *track, int, char * ) ;
extern void main_menu_init();
extern bool tuxkart_exit;
extern bool aiCheats;

static void switch_to_game ()
{
  int t ;
  int nl ;

  trackButtons -> getValue ( & t ) ;
  nl = atoi ( numLapsText->getLegend () ) ;

  //aiCheats = atoi(cheaterAIs->getStringValue());
  aiCheats = false;
  printf("aicheats = %i\n",aiCheats);

  puDeleteObject ( pleaseWaitButton ) ;
  //puDeleteObject ( cheaterAIs    ) ;
  puDeleteObject ( hostnameText  ) ;
  puDeleteObject ( numAIsSlider  ) ;
  puDeleteObject ( numAIsText    ) ;
  puDeleteObject ( numLapsSlider ) ;
  puDeleteObject ( numLapsText   ) ;
  puDeleteObject ( playButton    ) ;
  puDeleteObject ( exitButton    ) ;
  puDeleteObject ( trackButtons  ) ;
  delete intro_gst ;
  
  delete fnt_sorority  ;
  delete sorority      ;
  delete fnt_avantgarde;
  delete avantgarde    ;

  tuxkart_main ( nl, track_idents[t], numAIs, hostname ) ;

  /* we come back here after leaving the race */

  main_menu_init();
}

/*********************************\
*                                 *
* These functions capture mouse   *
* and keystrokes and pass them on *
* to PUI.                         *
*                                 *
\*********************************/

static void keyfn ( int key, int updown, int, int )
{
  puKeyboard ( key, updown ) ;
}

/*
static void motionfn ( int x, int y )
{
  puMouse ( x, y ) ;
}

static void mousefn ( int button, int updown, int x, int y )
{
  puMouse ( button, updown, x, y ) ;
}
*/

static void resizefn ( int w, int h )
{
  printf("%i, %i\n",w, h);
  width = w; height = h;
  pwSetSize ( width, height ) ;
}

/***********************************\
*                                   *
* This function redisplays the PUI, *
* and flips the double buffer.      *
*                                   *
\***********************************/

static void displayfn (void)
{
  glMatrixMode   ( GL_PROJECTION ) ;
  glLoadIdentity () ;
  glMatrixMode   ( GL_MODELVIEW ) ;
  glLoadIdentity () ;
  glDisable      ( GL_DEPTH_TEST ) ;
  glDisable      ( GL_LIGHTING   ) ;
  glDisable      ( GL_FOG        ) ;
  glDisable      ( GL_CULL_FACE  ) ;
  glDisable      ( GL_ALPHA_TEST ) ;
  glOrtho        ( 0, 640, 0, 480, 0, 100 ) ;

  intro_gst -> force () ;

  glBegin ( GL_QUADS ) ;
  glColor3f    ( 1, 1, 1 ) ;
  glTexCoord2f ( 0, 0 ) ; glVertex2i (   0,   0 ) ;
  glTexCoord2f ( 1, 0 ) ; glVertex2i ( 640,   0 ) ;
  glTexCoord2f ( 1, 1 ) ; glVertex2i ( 640, 480 ) ;
  glTexCoord2f ( 0, 1 ) ; glVertex2i (   0, 480 ) ;
  glEnd () ;

  /* Make PUI redraw */

  glEnable ( GL_BLEND ) ;
  puDisplay () ;
  
  /* Off we go again... */

  pwSwapBuffers   () ;

  if ( startup_counter > 0 )
  {
    if ( --startup_counter <= 0 )
      switch_to_game () ;
  }
}


/***********************************\
*                                   *
* Here are the PUI widget callback  *
* functions.                        *
*                                   *
\***********************************/

static void play_cb ( puObject * )
{
  puSetDefaultColourScheme ( 123.0f/255.0f, 0.0f/255.0f, 34.0f/255.0f, 1.0) ;
  
  pleaseWaitButton = new puButton ( 100, 240,
                               "LOADING: PLEASE WAIT FOR A MINUTE OR TWO"  ) ;

  hostname = hostnameText->getStringValue();
  /*if(strlen(hostname) != 0)
  {
    pleaseWaitButton->setLegend( "PLEASE MAKE SURE THE OTHER USER IS READY"  ) ;
  }*/

  startup_counter = 3 ;
}


static void exit_cb ( puObject * )
{
  fprintf ( stderr, "Exiting TuxKart starter program.\n" ) ;
  exit ( 1 ) ;
}



static void numLapsSlider_cb ( puObject *)
{
  float d ;

  numLapsSlider->getValue ( & d ) ;

  numLaps = 1 + (int)( d / 0.05f ) ;

  if ( numLaps <  1 ) numLaps =  1 ;
  if ( numLaps > 20 ) numLaps = 20 ;

  sprintf ( numLapsLegend, "%2d", numLaps ) ;
  numLapsText->setLegend ( numLapsLegend ) ;
}

static void numAIsSlider_cb ( puObject *)
{
  float d ;

  numAIsSlider->getValue ( & d ) ;

  numAIs = (int)( d / 0.25f ) ;

  if ( numAIs <  0 ) numAIs =  0 ;
  if ( numAIs >  3 ) numAIs =  3 ;

  sprintf ( numAIsLegend, "%2d", numAIs ) ;
  numAIsText->setLegend ( numAIsLegend ) ;
}

static void install_material ()
{
  intro_gst = new ssgSimpleState ;
 
  if ( getenv ( "MESA_GLX_FX" ) != NULL )
    intro_gst -> setTexture ( "images/title_screen_small.rgb", TRUE, TRUE ) ;
  else
    intro_gst -> setTexture ( "images/title_screen.rgb", TRUE, TRUE ) ;

  intro_gst -> enable      ( GL_TEXTURE_2D ) ;
  intro_gst -> disable     ( GL_LIGHTING  ) ;
  intro_gst -> disable     ( GL_CULL_FACE ) ;
  intro_gst -> setOpaque   () ;
  intro_gst -> disable     ( GL_BLEND ) ;
  intro_gst -> setShadeModel ( GL_SMOOTH ) ;
  intro_gst -> disable     ( GL_COLOR_MATERIAL ) ;
  intro_gst -> enable      ( GL_CULL_FACE      ) ;
  intro_gst -> setMaterial ( GL_EMISSION, 0, 0, 0, 1 ) ;
  intro_gst -> setMaterial ( GL_SPECULAR, 0, 0, 0, 1 ) ;
  intro_gst -> setMaterial ( GL_DIFFUSE, 0, 0, 0, 1 ) ;
  intro_gst -> setMaterial ( GL_AMBIENT, 0, 0, 0, 1 ) ;
  intro_gst -> setShininess ( 0 ) ;
}
                                                                                
static void loadTrackList ()
{
  char *fname = "data/levels.dat" ;

  if ( getenv ( "TUXKART_TRACKLIST" ) != NULL )
    fname = getenv ( "TUXKART_TRACKLIST" ) ;

  FILE *fd = fopen ( fname, "ra" ) ;

  if ( fd == NULL )
  {
    fprintf ( stderr, "tuxkart: Can't open '%s'\n", fname ) ;
    exit ( 1 ) ;
  }

  int i = 0 ;

  while ( i < MAX_TRACKS && ! feof ( fd ) )
  {
    char *p ;
    char  s [ 1024 ] ;

    if ( fgets ( s, 1023, fd ) == NULL )
      break ;
 
    if ( *s == '#' )
      continue ;

    for ( p = s ; *p > ' ' && *p != '\0' ; p++ )
      /* Search for a space */ ;

    if ( *p == ' ' )
    {
      *p = '\0' ;
      track_idents [ i ] = new char [ strlen(s)+1 ] ;
      strcpy ( track_idents [ i ], s ) ;
      p++ ;

      while ( *p <= ' ' && *p != '\0' )
        p++ ; 

      track_names [ i ] = new char [ strlen(p)+1 ] ;
      strcpy ( track_names [ i ], p ) ;

      i++ ;
    }
  }

  track_names  [ i ] = NULL ;
  track_idents [ i ] = NULL ;

  fclose ( fd ) ;
}


extern void cmdline_help();
extern bool network_enabled, network_testing;
bool netClient = false;
extern int fastForward;
extern bool demoMode;
extern int tuxkart_main_net(char * host);
int main ( int argc, char **argv )
{
  bool immediatePlay = false;
  char * trackName;
  //if(argv[1])
  for(int i = 1; argv[i]; ++i )
  {
    if(strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0)
    {
      cmdline_help();
      return 0;
    }
    else if(strcmp(argv[i],"-n") == 0)
    {
      network_enabled = TRUE;
    }
    else if(strcmp(argv[i],"-t") == 0)
    {
      trackName = argv[++i];
      immediatePlay = true;
    }
    else if(strcmp(argv[i],"-d") == 0)
    {
      demoMode = true;
      immediatePlay = true;
    }
    else if(strcmp(argv[i],"-l") == 0)
    {
      numLaps = atoi ( argv[++i] );
    }
    else if(strcmp(argv[i],"-k") == 0)
    {
      numAIs = atoi ( argv[++i] ) ;
    }
    else if(strcmp(argv[i],"--fastforward") == 0)
    {
      fastForward = atoi ( argv[++i] ) ;
    }
    /*else
    {
      network_enabled = TRUE;
      netClient = true;
    }*/
  }

  /* Set tux_aqfh_datadir to the correct directory */
 
  if ( datadir == NULL )
  {
    if ( getenv ( "TUXKART_DATADIR" ) != NULL )
      datadir = getenv ( "TUXKART_DATADIR" ) ;
    else
#ifdef _MSC_VER
    if ( _access ( "data/levels.dat", 04 ) == 0 )
#else
    if ( access ( "data/levels.dat", F_OK ) == 0 )
#endif
      datadir = "." ;
    else
#ifdef _MSC_VER
    if ( _access ( "../data/levels.dat", 04 ) == 0 )
#else
    if ( access ( "../data/levels.dat", F_OK ) == 0 )
#endif
      datadir = ".." ;
    else
#ifdef TUXKART_DATADIR
      datadir = TUXKART_DATADIR ;
#else
      datadir = "/usr/local/share/games/tuxkart" ;
#endif
  }
 
  fprintf ( stderr, "Data files will be fetched from: '%s'\n",
                                                    datadir ) ;
 
#ifdef _MSC_VER
  if ( _chdir ( datadir ) == -1 )
#else
  if ( chdir ( datadir ) == -1 )
#endif
  {
    fprintf ( stderr, "Couldn't chdir() to '%s'.\n", datadir ) ;
    exit ( 1 ) ;
  }                                                                             

  loadTrackList () ;

  pwInit ( 0, 0, width, height, FALSE, "Classic Tux Kart", TRUE, 0 ) ;

  pwSetCallbacks ( keyfn, mousefn, motionfn, resizefn, NULL ) ;

  puInit () ;
  ssgInit () ;

  sound = new SoundSystem ;

  if(immediatePlay)
  {
    tuxkart_main ( numLaps, trackName, numAIs, hostname ) ;
  }

/*
  if(netClient)
  {
    return tuxkart_main_net( argv[1] );
  }
*/

  main_menu_init();
  

  while ( 1 )
    displayfn () ;

  return 0 ;
}

void main_menu_init()
{

  fnt_sorority = new fntTexFont ;
  fnt_sorority -> load ( "fonts/sorority.txf" ) ;
  sorority = new puFont ( fnt_sorority, 12 ) ;
  
  fnt_avantgarde = new fntTexFont ;
  fnt_avantgarde -> load ( "fonts/AvantGarde-Demi.txf" ) ;
  avantgarde = new puFont ( fnt_avantgarde, 11 ) ;

  puSetDefaultFonts        ( *sorority, *sorority ) ;
  puSetDefaultStyle        ( PUSTYLE_SMALL_SHADED ) ;
  puSetDefaultColourScheme ( 243.0f/255.0f, 140.0f/255.0f, 34.0f/255.0f, 1.0) ;

  playButton = new puButton     ( 10, 10, 150, 50 ) ;
  playButton->setLegend         ( "Start Game"  ) ;
  playButton->setCallback       ( play_cb ) ;
  playButton->makeReturnDefault ( TRUE ) ;

  exitButton = new puButton     ( 180, 10, 250, 50 ) ;
  exitButton->setLegend         ( "Quit"  ) ;
  exitButton->setCallback       ( exit_cb ) ;


  numAIsSlider = new puSlider  ( 10, 130, 150 ) ;
  numAIsSlider->setLabelPlace ( PUPLACE_ABOVE ) ;
  numAIsSlider->setLabel  ( "How Many AIs?" ) ;
  numAIsSlider->setDelta  ( 0.40 ) ;
  numAIsSlider->setCBMode ( PUSLIDER_ALWAYS ) ;
  numAIsSlider->setValue  ( ((numAIs)*0.28f) ) ;
  numAIsSlider->setCallback ( numAIsSlider_cb ) ;

  numAIsText = new puButton ( 160, 130, " 3" ) ;
  numAIsText->setStyle ( PUSTYLE_BOXED ) ;

  sprintf ( numAIsLegend, "%2d", numAIs ) ;
  numAIsText->setLegend ( numAIsLegend ) ;


  numLapsSlider = new puSlider  ( 10, 80, 150 ) ;
  numLapsSlider->setLabelPlace ( PUPLACE_ABOVE ) ;
  numLapsSlider->setLabel  ( "How Many Laps?" ) ;
  numLapsSlider->setDelta  ( 0.05 ) ;
  numLapsSlider->setCBMode ( PUSLIDER_ALWAYS ) ;
  numLapsSlider->setValue  ( ((numLaps-1)*0.05f) ) ;
  numLapsSlider->setCallback ( numLapsSlider_cb ) ;

  numLapsText = new puButton ( 160, 80, " 5" ) ;
  numLapsText->setStyle ( PUSTYLE_BOXED ) ;

  sprintf ( numLapsLegend, "%2d", numLaps ) ;
  numLapsText->setLegend ( numLapsLegend ) ;


  trackButtons = new puButtonBox ( 400, 8, 630, 167, track_names, TRUE ) ;
  trackButtons -> setLabel ( "Which Track?" ) ;
  trackButtons -> setLabelPlace ( PUPLACE_ABOVE ) ;
  trackButtons -> setValue ( 0 ) ; 


  /*
  cheaterAIs = new puButton ( 204, 130, 384, 160 , PUBUTTON_NORMAL ) ;
  cheaterAIs->setLegend     ( "Classic AI Speedup" ) ;
  */


  hostnameText = new puInput  ( 204, 80, 384, 110 ) ;
  hostnameText->setLabelPlace ( PUPLACE_ABOVE ) ;
  hostnameText->setLabel  ( "Hostname" ) ;
  hostnameText->setLegendFont   ( *avantgarde ) ;
  hostnameText->setValue  ( "" ) ;

  sound -> change_track ( "oggs/main_theme.ogg" ) ;

  install_material () ;

  signal ( 11, SIG_DFL ) ;
}


