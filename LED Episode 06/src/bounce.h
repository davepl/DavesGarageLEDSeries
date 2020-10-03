//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:        bounce.h
//
// Description:
//
//   Draws bouncing balls on an LED strip
//
// History:     Ocf-01-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <sys/time.h>                               // For time-of-day

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

using namespace std;                        
#include <vector>                               // Use the C++ resizable array

extern CRGB g_LEDs[];

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))   // Count elements in a static array

// BouncingBallEffect
//
// Draws a set of N bouncing balls using a simple little kinematics formula.  Clears the section first.

static const CRGB ballColors[] =
{
	CRGB::Green,
	CRGB::Red,
	CRGB::Blue,
	CRGB::Orange,
	CRGB::Indigo,
	CRGB::Cyan
};

class BouncingBallEffect 
{
private:

	size_t	_iOffset;
	size_t  _cLength;
	size_t  _cBalls;
  byte    _Fade;
	bool    _bMirrored;

	const double Gravity = -9.81;
	const double StartHeight = 1;
	const double ImpactVelocityStart = sqrt(-2 * Gravity * StartHeight);
	
	vector<double> ClockTimeAtLastBounce, TimeSinceLastBounce, Height, BallSpeed, Dampening;
	vector<CRGB> Colors;
	 
  // Time - Return current time in the floating form popular under Unix, which is the number of seconds 
  //        since January 1, 1970.  That way we can treat the elapsed time as simple seconds.

  double UnixStyleTime() const
  {
    timeval tv = { 0 };
    gettimeofday(&tv, nullptr);
    return tv.tv_usec / (double)1000000 +(double)tv.tv_sec;
  }

public:

	BouncingBallEffect(size_t cLength, size_t ballCount = 3, byte fade = 0, bool bMirrored = true)
		: _cLength(cLength-1),                // Reserve one LED for floating point fraction draw
		  _cBalls(ballCount),                 // The number of balls
      _Fade(fade),                        // The rate at which to fade the strip between passes
		  _bMirrored(bMirrored),              // If we should mirror the drawing from both edges
      ClockTimeAtLastBounce(ballCount),   // Size the arrays we need to hold data for each ball
   		Height(ballCount),
	  	BallSpeed(ballCount),
		  Dampening(ballCount),
		  Colors(ballCount)
	{
		for (size_t i = 0; i < ballCount; i++)
		{
			Height[i] 					        = StartHeight;
			ClockTimeAtLastBounce[i]    = UnixStyleTime();
			Dampening[i] 				        = 0.90 - i / pow(_cBalls, 2);
			BallSpeed[i] 			          = ImpactVelocityStart * Dampening[i];
			TimeSinceLastBounce[i] 		  = 0;
			Colors[i] 					        = ballColors[i % ARRAYSIZE(ballColors)];
		}
	}

	// Draw
	//
	// Draw each of the balls.  When any ball gets too little energy it would just sit at the base so it is re-kicked with new energy.#pragma endregion

  virtual void Draw()
  {
    if (_Fade)
    {
      for (size_t i = 0; i < _cLength; i++)
      {
        g_LEDs[i].fadeToBlackBy(_Fade);     
      }
    }
    else
    {
      FastLED.clear();
    }

    // Draw each of balls
    for (size_t i = 0; i < _cBalls; i++)
    {     
      TimeSinceLastBounce[i] = (UnixStyleTime() - ClockTimeAtLastBounce[i]);
      Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce[i], 2.0) + BallSpeed[i] * TimeSinceLastBounce[i];

      if (Height[i] < 0)
      {
        Height[i] = 0;
        BallSpeed[i] = Dampening[i] * BallSpeed[i];
        ClockTimeAtLastBounce[i] = UnixStyleTime();

        if (BallSpeed[i] < 0.01)
          BallSpeed[i] = ImpactVelocityStart;
      }

      size_t position = (size_t) round(Height[i] * (_cLength - 1) / StartHeight);
      
      g_LEDs[position]   = Colors[i];
      g_LEDs[position+1] = Colors[i];
      
      if (_bMirrored) 
      {
        g_LEDs[_cLength - 1 - position] = Colors[i];
        g_LEDs[_cLength -  position]    = Colors[i];
      }
    }
    delay(20);
  }
};
