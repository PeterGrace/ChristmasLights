#include <ArduinoJson.h>

#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "auth.h"

#define BOARD TM1803
#define PIN 5
#define NUM_LEDS 10
#define COLOR_ORDER RBG

#define FADE_VALUE 10

#define FRAMES_PER_SEC 60

CRGB leds[NUM_LEDS];

DynamicJsonBuffer jsonBuffer;

//mode settings
int mode_setting=0;

// voob settings
int voob_bpm=20;
int voob_color=0;

// icicle settings
int icicle_pos=0;
bool icicle_animate=false;
bool icicle_pauser=false;

static int min_millis=1000/FRAMES_PER_SEC;

unsigned int udp_port = 1225;
char incomingPacket[255];

WiFiUDP udp;

void initialize_wifi()
{
  char msg[255];
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  sprintf(msg, "Trying to connect to ssid %s with password %s.\n", ssid, password);
  Serial.print(msg);
  while (WiFi.localIP().toString() == "0.0.0.0")
  {
    delay(500);
    Serial.println(WiFi.localIP().toString());
  }
  Serial.print("Connected to wifi with ip");
  Serial.println(WiFi.localIP());
  delay(1000);
  WiFi.setAutoReconnect(true);
  udp.begin(udp_port);
}



void setup() {
  // put your setup code here, to run once:
 Serial.begin(74880);
 delay(3000); 
 FastLED.addLeds<BOARD, PIN, COLOR_ORDER>(leds, NUM_LEDS);
 fill_solid(leds, NUM_LEDS, CRGB::Blue);
 FastLED.show();
 initialize_wifi();
 Serial.print("\n\nInitialized.  Frame rate dictates minimum refresh rate of: ");
 Serial.println(min_millis);
 randomSeed(analogRead(0));
 
}

void random_colors()
{
    EVERY_N_MILLISECONDS(1250)
    {
      if (leds[0].getLuma() < 1)
      {
        voob_color = random(0,4);
      }
    }
}

void solid_mode()
{
  uint8_t bright = 255;
    switch (voob_color)
    {
      case 0: {
                //red hue
                fill_solid(leds, NUM_LEDS, CHSV(0,255,bright));
                break;
              }
      case 1: {
                //green hue
                fill_solid(leds, NUM_LEDS, CHSV(96,255,bright));
                break;
              }
      case 2: {
                //blue hue
                fill_solid(leds, NUM_LEDS, CHSV(160,255,bright));
                break;
              }
      case 3: {
                //orange hue
                fill_solid(leds, NUM_LEDS, CHSV(32,255,bright));
                break;
              }
      }

}

void voob_mode()
{
    uint8_t bright = beatsin8(voob_bpm,0,255);
    switch (voob_color)
    {
      case 0: {
                //red hue
                fill_solid(leds, NUM_LEDS, CHSV(0,255,bright));
                break;
              }
      case 1: {
                //green hue
                fill_solid(leds, NUM_LEDS, CHSV(96,255,bright));
                break;
              }
      case 2: {
                //blue hue
                fill_solid(leds, NUM_LEDS, CHSV(160,255,bright));
                break;
              }
      case 3: {
                //orange hue
                fill_solid(leds, NUM_LEDS, CHSV(32,255,bright));
                break;
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

void red_voob()
{
  uint8_t bright = beatsin8(50,0,255);
  fill_solid(leds, NUM_LEDS, CHSV(0,255,bright));
}

void process_packet()
{
  Serial.print("Received packet: ");
  Serial.println(incomingPacket);
  JsonObject& input = jsonBuffer.parseObject(incomingPacket);
  if (input["mode"])
    mode_setting=input["mode"];
  if (input["bpm"])
    voob_bpm=input["bpm"];  
}
  

void black()
{
  fadeToBlackBy(leds, NUM_LEDS, 100);
}

void modeSelection()
{
  switch (mode_setting)
  {
    case 1: {
                // choose red, green, blue or orange to voob
                random_colors();
                voob_mode();
                break;
             }
    case 2: {
                icicle_mode();
                break;
             }
    case 10: {
                //red voob only
                voob_color=0;
                voob_mode();
                break;
             }
    case 11: { 
                voob_color=0;
                solid_mode();
                break;         
             }
    case 20: {
                //green voob only
                voob_color=1;
                voob_mode();
                break;
             }
    case 21: { 
                voob_color=1;
                solid_mode();
                break;         
             }

    case 30: {
                //blue voob only
                voob_color=2;
                voob_mode();
                break;
             }
    case 31: { 
                voob_color=2;
                solid_mode();
                break;         
             }

    case 40: {
                //orange voob only
                voob_color=3;
                voob_mode();
                break;
             }
    case 41: { 
                voob_color=3;
                solid_mode();
                break;         
             }
             
             
             

    default: {
                black();
                break;
             }
    }
}

void udpCheck()
{
  //udp handling
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
      int len = udp.read(incomingPacket, 255);
      if (len >0)
      {
        incomingPacket[len]=0;
      }
      process_packet();
    }     
  
}
void loop() {  
  FastLED.delay(min_millis);
  udpCheck();
  modeSelection();
}


