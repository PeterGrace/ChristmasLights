#include <FastLED.h>

#define BOARD TM1803
#define PIN 5
#define NUM_LEDS 10
#define COLOR_ORDER RBG

#define FADE_VALUE 10

#define FRAMES_PER_SEC 60

CRGB leds[NUM_LEDS];


//mode settings
int mode_setting=1;


// icicle settings
int icicle_pos=0;
bool icicle_animate=false;
bool icicle_pauser=false;

static int min_millis=1000/FRAMES_PER_SEC;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 delay(3000); 
 FastLED.addLeds<BOARD, PIN, COLOR_ORDER>(leds, NUM_LEDS);
 fill_solid(leds, NUM_LEDS, CRGB::Blue);
 FastLED.show();
 Serial.print("\n\nInitialized.  Frame rate dictates minimum refresh rate of: ");
 Serial.println(min_millis);
 randomSeed(analogRead(0));
 
}

void random_colors()
{
    EVERY_N_MILLISECONDS(min_millis)
  {
    fadeToBlackBy( leds, NUM_LEDS, FADE_VALUE);     
  }
    EVERY_N_MILLISECONDS(1250)
  {
    int rando = random(0,4);
    Serial.print("Setting random to: ");
    Serial.println(rando);
    switch (rando)
    {
      case 0: {
                fill_solid(leds, NUM_LEDS, CRGB::Red);
                break;
              }
      case 1: {
                fill_solid(leds, NUM_LEDS, CRGB::Green);
                break;
              }
      case 2: {
                fill_solid(leds, NUM_LEDS, CRGB::Blue);
                break;
              }
      case 3: {
                fill_solid(leds, NUM_LEDS, CRGB::Orange);
                break;
              }
      }
    }

}

void icicle_mode()
{
  EVERY_N_MILLISECONDS(random(2000,5000))
  {
    icicle_animate=true;
  }

  EVERY_N_MILLISECONDS(random(min_millis, 300))
  {
    if (icicle_pauser==false)
    {
      icicle_animate=false;
      icicle_pauser=true;
    }
    else
    {
      icicle_animate=true;
      icicle_pauser=false;
    }
  }
  
  if (icicle_animate)
  {
    fadeToBlackBy( leds, NUM_LEDS, 100);  
    leds[icicle_pos] = CHSV(160,128,255);
    icicle_pos++;
    if (icicle_pos > NUM_LEDS+1)
    {
      icicle_animate = false;
      icicle_pauser = false;
      icicle_pos=0;
      fill_solid(leds, NUM_LEDS, CRGB::Black);
    }
  }
}



void loop() {
  FastLED.delay(min_millis);
  switch (mode_setting)
  {
    case 0: {
                random_colors();
                break;
             }
    case 1: {
                icicle_mode();
                break;
             }

    default: {
                break;
             }
    }
  }


