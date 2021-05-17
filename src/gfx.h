
class GFX
{
public:

   GFX () ;
   void update () ;
   void done   () ;

   sgVec3 sunposn   ;
   sgVec4 skyfogcol ;
   sgVec4 ambientcol ;
   sgVec4 specularcol ;
   sgVec4 diffusecol ;
} ;


int stereoShift () ;
void reshape ( int x, int y ) ;
void keystroke ( int key, int updown, int x, int y ) ;
