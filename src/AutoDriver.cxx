
#include "tuxkart.h"


inline float sgnsq ( float x ) { return ( x < 0 ) ? -(x * x) : (x * x) ; }


bool aiCheats;
void AutoKartDriver::update ()
{
  /* Steering algorithm */

  /* If moving left-to-right and on the left - or right to left
     and on the right - do nothing. */

  sgVec2 track_velocity ;
  sgSubVec2 ( track_velocity, curr_track_coords, last_track_coords ) ;

  if ( ( track_velocity [ 0 ] < 0.0f && curr_track_coords [ 0 ] > 0.0f ) ||
       ( track_velocity [ 0 ] > 0.0f && curr_track_coords [ 0 ] < 0.0f ) )
    velocity.hpr[0] = sgnsq(curr_track_coords[0])*3.0f ;
  else
    velocity.hpr[0] = sgnsq(curr_track_coords[0])*12.0f ;

  //if ( velocity.hpr[0] > 20 ) velocity.hpr[0] = 20 ;
  while ( fabs ( velocity.hpr[0] ) > 300 )
    velocity.hpr[0] *= 0.9f ;

  if(aiCheats)
  {
    /* Slow down if we get too far ahead of the player... */
    if ( position < kart[0]->getPosition () &&
         velocity.xyz[1] > MIN_HANDICAP_VELOCITY )
      velocity.xyz[1] -= MAX_BRAKING * delta_t * 0.1f ;
    else
    
    /* Speed up if we get too far behind the player... */
    if ( position > kart[0]->getPosition () &&
         velocity.xyz[1] < MAX_HANDICAP_VELOCITY )
      velocity.xyz[1] += MAX_ACCELLERATION * delta_t * 1.1f ;
    else
      velocity.xyz[1] += MAX_ACCELLERATION * delta_t ;
  }
  else
  {
    /* Wheelie time */
    
    if ( position >= fmin( 2, kart[0]->getPosition() ) &&
         velocity.xyz[1] >= MIN_WHEELIE_VELOCITY ) 
    {
      if ( wheelie_angle < WHEELIE_PITCH )
        wheelie_angle += WHEELIE_PITCH_RATE * delta_t ;
      else 
        wheelie_angle = WHEELIE_PITCH ;
    }
    else if ( wheelie_angle > 0.0f )
    {
      wheelie_angle -= PITCH_RESTORE_RATE ;

      if ( wheelie_angle <= 0.0f )
        wheelie_angle = 0.0f ;
    }
    
    if ( position+4 < kart[0]->getPosition () &&
         velocity.xyz[1] > MIN_HANDICAP_VELOCITY )
      velocity.xyz[1] -= MAX_BRAKING * delta_t * 0.1f ;
    /*else if(velocity.xyz[1] < MAX_NATURAL_VELOCITY *
                    ( 1.0f + wheelie_angle/90.0f ))
      velocity.xyz[1] += MAX_ACCELLERATION * delta_t ;*/
    else
    {
      velocity.xyz[1] += fmin( MAX_ACCELLERATION * ( 1.0f + velocity.xyz[1]/360.0f ) , MAX_NATURAL_VELOCITY * ( 1.0f + wheelie_angle/90.0f ) - velocity.xyz[1] - ( curr_pos.hpr[1] / 3.f ) ) * delta_t ;
    }
  }

  curr_pos.hpr[1] -= curr_pos.hpr[1]*0.025f;
  curr_pos.hpr[2] -= curr_pos.hpr[2]*0.025f;
  velocity.xyz[2] -= GRAVITY * delta_t ;

  float wheelieMargin = 8.f;
  if(position > kart[0]->getPosition ())
    wheelieMargin = 16.f;

  if ( fabs(velocity.hpr[0]) > wheelieMargin && wheelie_angle > 0.0f )
  {
    wheelie_angle -= PITCH_RESTORE_RATE ;
 
    if ( wheelie_angle <= 0.0f )
      wheelie_angle = 0.0f ;
  }

  if ( collectable != COLLECT_NOTHING )
  {
    time_since_last_shoot += delta_t ;

    if ( time_since_last_shoot > 10.0f )
    {
      useAttachment () ;
      time_since_last_shoot = 0.0f ;
    }
  }

  KartDriver::update () ;
}


void NetworkKartDriver::update ()
{
  
  // wheelie detection fix
  if ( velocity.xyz[1] > MAX_NATURAL_VELOCITY && velocity.xyz[0] == 0.f ) 
  {
    if ( wheelie_angle < WHEELIE_PITCH )
      wheelie_angle += WHEELIE_PITCH_RATE * delta_t ;
    else 
      wheelie_angle = WHEELIE_PITCH ;
  }
  else if ( wheelie_angle > 0.0f )
  {
    wheelie_angle -= PITCH_RESTORE_RATE ;

    if ( wheelie_angle <= 0.0f )
      wheelie_angle = 0.0f ;
  }
  
  KartDriver::update () ;
}


