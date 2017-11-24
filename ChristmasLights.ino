#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

#include "auth.h"

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

#define BOARD WS2812B
#define PIN 5
#define COLOR_ORDER GRB

#define FADE_VALUE 10

#define FRAMES_PER_SEC 60

#define MAX_MSG_SIZE 255

elapsedMillis em_status_screen=0;
elapsedMillis em_icicle_velocity=0;
elapsedMillis em_icicle_new=0;
elapsedMillis em_pause=0;


uint8_t global_brightness=255;

unsigned long pause_till=0;
unsigned long _uptime=0;

int status_screen_interval=10000;


#define VALID_VER 124

struct cl_config {
  int mode;
  int bpm;
  int brightness;
  int num_leds;
} config;

const char newicicle[]="Starting new icicle";
const char new_begin[]="Beginning new icicle timer";
const char new_animate[]="Beginning velocity timer";
const char icicledelay[]="icicle was due to start, but we're still animating.  Resetting timer.";
const char trymsg[]="Trying to connect to wifi";
CRGB leds[1024];
CRGB endclr, midclr;

WiFiUDP UDP;

DynamicJsonBuffer jsonBuffer;
static int min_millis=1000/FRAMES_PER_SEC;

int newtime=0;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

long bpm_millis=0;

// voob settings
int voob_color=0;

// icicle settings
int icicle_pos=0;
int icicle_velocity_interval=0;
int icicle_new_interval=0;
bool icicle_animate=false;
int icicle_new_min=5000;
int icicle_new_max=10000;
int icicle_wait_min=min_millis;
int icicle_wait_max=10000;

//swipe mode
int swipe_sin1 = 0;
int swipe_sin2 = 0;

// the addresses we will use to store integers in.
#define ADDR_VALID 0
#define ADDR_CONFIG 1
#define EEPROM_SIZE 64
uint8_t gHue = 0;

int chase_pos=0;

void initialize_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  debugmsg(trymsg);
  char progressmsg[MAX_MSG_SIZE];
  while (WiFi.localIP().toString() == "0.0.0.0")
  {
    delay(500);
    sprintf(progressmsg,"IP: %s",WiFi.localIP().toString().c_str());
    debugmsg(progressmsg);
  }
  int newmillis = millis()+3000;
  while (newmillis > millis())
    delay(1);
  sprintf(progressmsg,"Connected to wifi with ip %s", WiFi.localIP().toString().c_str());
  debugmsg(progressmsg);
  WiFi.setAutoReconnect(true);
}

unsigned long calc_uptime()
{
  _uptime = _uptime + (millis() - _uptime);
  return _uptime / 1000;
}

void setup() {
  // put your setup code here, to run once:
 EEPROM.begin(EEPROM_SIZE);
 Serial.begin(74880);
 initialize_wifi(); 
 delay(3000); 
 load_eeprom_values(); 
 
 MDNS.begin(WiFi.hostname().c_str());
 httpUpdater.setup(&httpServer);
 httpServer.on("/lights", HTTP_POST, process_post);
 httpServer.on("/reboot/please", HTTP_POST, [](){httpServer.send ( 200, "text/json", "{\"done\":true}" );ESP.restart();});
 httpServer.on("/eeprom/write", HTTP_POST, [](){httpServer.send ( 200, "text/json", "{\"done\":true}" );cl_write_eeprom();});
 httpServer.on("/eeprom/read", HTTP_POST, [](){httpServer.send ( 200, "text/json", "{\"done\":true}" );validate_eeprom();});
 
 httpServer.begin();
 MDNS.addService("http", "tcp", 80);
 

 FastLED.addLeds<BOARD, PIN, COLOR_ORDER>(leds, config.num_leds);
 set_brightness(255);
 char initialized[MAX_MSG_SIZE];
 sprintf(initialized,"Initialized.  Frame rate dictates minimum refresh rate of: %d", min_millis);
 debugmsg(initialized);
 randomSeed(analogRead(0));
}

void cl_write_eeprom()
{
  char eeprommsg[MAX_MSG_SIZE];
  EEPROM.write(ADDR_VALID, VALID_VER);
  EEPROM.put(ADDR_CONFIG, config);
  EEPROM.commit();
  sprintf(eeprommsg, "Saved to eeprom: m:%d, bpm:%d, bright:%d,leds:%d\n",config.mode, config.bpm, config.brightness,config.num_leds);
  debugmsg(eeprommsg);
  validate_eeprom();
}

void validate_eeprom()
{
  char eeprommsg[MAX_MSG_SIZE];
  EEPROM.get(ADDR_CONFIG, config);
  sprintf(eeprommsg, "Read from eeprom: m:%d, bpm:%d, bright:%d, leds:%d\n",config.mode, config.bpm, config.brightness,config.num_leds);
  debugmsg(eeprommsg);
}

void load_eeprom_values()
{
  int _valid, _bpm, _bright, _mode=0;
  _valid = EEPROM.read(ADDR_VALID);
  char eeprommsg[MAX_MSG_SIZE];
  sprintf(eeprommsg,"Valid came back as %d\n", _valid);    
  debugmsg(eeprommsg);
  if (_valid == VALID_VER)
  {
    EEPROM.get(ADDR_CONFIG, config);    
    sprintf(eeprommsg, "EEPROM was valid\n");
    debugmsg(eeprommsg);
  }
  else
  {
    config.bpm = 60;
    config.brightness = 255;
    config.mode = 3;
    config.num_leds = 123;
    cl_write_eeprom();
    sprintf(eeprommsg, "Wrote defaults to eeprom\n",_mode,_bpm,_bright);
    debugmsg(eeprommsg);

  }
  set_bpm(config.bpm);
  set_brightness(config.brightness);
  config.mode = config.mode;

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

void confetti_mode() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, config.num_leds, 10);
  int pos = random16(config.num_leds);
  leds[pos] += CHSV( random8(255), 200, 255);
}

void rainbow_solid()
{
  fill_rainbow(leds, config.num_leds, 0, 7);
}

void rainbow_bpm()
{
  uint8_t beatA = beatsin8(config.bpm, 0, 255);
  fill_rainbow(leds, config.num_leds, beatA, 7);
}

void murica_chase()
{
  fadeToBlackBy( leds, config.num_leds, 10);
  uint8_t pos=beatsin8(config.bpm,1,config.num_leds-1);
  leds[pos-1]=CRGB(CRGB::Red);
  leds[pos]=CRGB(CRGB::White);
  leds[pos+1]=CRGB(CRGB::Blue);
}

void solid_mode()
{
  uint8_t bright = 255;
    switch (voob_color)
    {
      case 0: {
                //red hue
                fill_solid(leds, config.num_leds, CHSV(0,255,bright));
                break;
              }
      case 1: {
                //green hue
                fill_solid(leds, config.num_leds, CHSV(96,255,bright));
                break;
              }
      case 2: {
                //blue hue
                fill_solid(leds, config.num_leds, CHSV(160,255,bright));
                break;
              }
      case 3: {
                //orange hue
                fill_solid(leds, config.num_leds, CHSV(32,255,bright));
                break;
              }
      }

}

void voob_mode()
{
    uint8_t bright = beatsin8(config.bpm,0,255);
    switch (voob_color)
    {
      case 0: {
                //red hue
                fill_solid(leds, config.num_leds, CHSV(0,255,bright));
                break;
              }
      case 1: {
                //green hue
                fill_solid(leds, config.num_leds, CHSV(96,255,bright));
                break;
              }
      case 2: {
                //blue hue
                fill_solid(leds, config.num_leds, CHSV(160,255,bright));
                break;
              }
      case 3: {
                //orange hue
                fill_solid(leds, config.num_leds, CHSV(32,255,bright));
                break;
              }
      }

}

void doIcicle()
{
  if (icicle_animate)
  {
    fadeToBlackBy( leds, config.num_leds, 100);  
    leds[icicle_pos] = CHSV(160,128,255);
    icicle_pos++;
    if (icicle_pos > config.num_leds+1)
    {
      icicle_animate = false;
      icicle_pos=0;
      fill_solid(leds, config.num_leds, CRGB::Black);
    }
  }
}

void setIcicleAnimate()
{
  if (icicle_animate==false)
  {
    debugmsg(newicicle);
    icicle_animate=true;
    icicle_new_interval=random(icicle_new_min, icicle_new_max);
    char newtimer[MAX_MSG_SIZE];
    sprintf(newtimer,"Icicle new timer set to: %d",icicle_new_interval);
    debugmsg(newtimer);
  }
  else
  {
    debugmsg(icicledelay);
    icicle_new_interval=random(icicle_new_min, icicle_new_max);
    char newtimer[MAX_MSG_SIZE];
    sprintf(newtimer,"Icicle new timer set to: %d",icicle_new_interval);
    debugmsg(newtimer);    
  }
}

void icicle_mode()
{
  if (icicle_velocity_interval<=0)
  {
    debugmsg(new_animate);
    icicle_velocity_interval=bpm_millis;
  }
  if (icicle_new_interval<=0)
  {
    debugmsg(new_begin);
    
    icicle_new_interval=random(icicle_new_min, icicle_new_max);
  }
  if (em_icicle_new > icicle_new_interval)
  {
    setIcicleAnimate();
    em_icicle_new=0;
  }
  if (em_icicle_velocity > icicle_velocity_interval)
  {
    doIcicle();
    em_icicle_velocity=0;
  }
  
}

void red_voob()
{
  uint8_t bright = beatsin8(config.bpm,0,255);
  fill_solid(leds, config.num_leds, CHSV(0,255,bright));
}

  

void black()
{
  fadeToBlackBy(leds, config.num_leds, 100);
}

void modeSelection()
{
  switch (config.mode)
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
    case 3: {
                confetti_mode();
                break;
            }         
    case 4: {
                rainbow_solid();
                break;
            } 
    case 5: {
                rainbow_bpm();
                break;
    }                   
    case 6: {
                murica_chase();
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
    case 50: {
               swipe_redgreen();        
               break;
              }
    case 51: {
               swipe_redblue();        
               break;
              }
    case 52: {
               swipe_bluegreen();        
               break;
              }
    case 99: { 
               gradient_mode();
               break;
            }
             
             
             

    default: {
                black();
                break;
             }
    }
}

void swipe_redgreen()
{
    swipe_sin1=beatsin8(config.bpm, 0, config.num_leds,0,0);
    fill_gradient_RGB(leds, 0, CRGB::Green, swipe_sin1, CRGB::Red);
    fill_gradient_RGB(leds, swipe_sin1, CRGB::Red, config.num_leds, CRGB::Green);
}

void swipe_redblue()
{
    swipe_sin1=beatsin8(config.bpm, 0, config.num_leds,0,0);
    fill_gradient_RGB(leds, 0, CRGB::Blue, swipe_sin1, CRGB::Red);
    fill_gradient_RGB(leds, swipe_sin1, CRGB::Red, config.num_leds, CRGB::Blue);
}

void swipe_bluegreen()
{
    swipe_sin1=beatsin8(config.bpm, 0, config.num_leds,0,0);
    fill_gradient_RGB(leds, 0, CRGB::Blue, swipe_sin1, CRGB::Green);
    fill_gradient_RGB(leds, swipe_sin1, CRGB::Green, config.num_leds, CRGB::Blue);
}


void gradient_mode()
{
  
  uint8_t speed = beatsin8(config.bpm,0,config.num_leds);
  endclr = blend(CRGB::Red, CRGB::Green, speed);
  midclr = blend(CRGB::Green, CRGB::Blue, speed);
  fill_gradient_RGB(leds, 0, endclr, config.num_leds/2, midclr);
  fill_gradient_RGB(leds, config.num_leds/2+1, midclr, config.num_leds, endclr);  
}


void loop() {  
  FastLED.delay(min_millis);
  if ((pause_till >=0) && (em_pause <pause_till))
  {
    fadeToBlackBy(leds, config.num_leds,100);
  } else if ((pause_till <= 0) || (em_pause >=pause_till))
  {
    em_pause = 0;
    pause_till=0;
    modeSelection();    
  };
  if (em_status_screen > status_screen_interval)
  {
    debugmsg("Still alive!");
    em_status_screen=0;
  }
  // Networky stuff
  httpServer.handleClient();

}

void set_bpm(int _bpm)
{
  char bpmmsg[MAX_MSG_SIZE];

  config.bpm = _bpm;
  bpm_millis = 60000/config.bpm;

  sprintf(bpmmsg, "BPM set to %d (millis: %d)", config.bpm, bpm_millis);
  debugmsg(bpmmsg);
}




void process_post()
{
  char msg[MAX_MSG_SIZE];
  JsonObject& input = jsonBuffer.parseObject(httpServer.arg("plain"));
  if (input["mode"])
    config.mode=input["mode"];
  if (input["sync_start"])
  { 
    em_pause=0;
    pause_till=atol(input["sync_start"]);
  }
  if (input["brightness"])
    set_brightness(atoi(input["brightness"]));
  if (input["bpm"])
  {
    set_bpm(atoi(input["bpm"]));
    icicle_velocity_interval=bpm_millis;
  }
  if (input["icicle_new_min"])
  {
    icicle_new_min=input["icicle_new_min"];
    if (icicle_new_min < min_millis)
      icicle_new_min=min_millis;
    if (icicle_new_interval)
    {
      icicle_new_interval=random(icicle_new_min, icicle_new_max);
      sprintf(msg,"Icicle new timer now is: %d", newtime); 
      debugmsg(msg);
    }
  }
  if (input["icicle_new_max"])
  {
    icicle_new_max=input["icicle_new_max"];
        if (icicle_new_interval)
    {
      icicle_new_interval=random(icicle_new_min, icicle_new_max);
      sprintf(msg,"Icicle new timer now is: %d", newtime); 
      debugmsg(msg);
    }

  }
  if (input["icicle_wait_min"])
  {
    icicle_wait_min=input["icicle_wait_min"];
    if (icicle_wait_min < min_millis)
      icicle_wait_min=min_millis;
  }
  if (input["icicle_wait_max"])
    icicle_wait_min=input["icicle_wait_max"];      
  if (input["num_leds"])
    {
      sprintf(msg,"Received %s from packet, writing %d to config.\n",input["num_leds"], atoi(input["num_leds"]));
      debugmsg(msg);
      config.num_leds = atoi(input["num_leds"]);
    }
    httpServer.send ( 200, "text/json", "{\"done\":true}" );    
}

void set_brightness(uint8_t brightness)
{
  char msg[MAX_MSG_SIZE];
  global_brightness=brightness;
  sprintf(msg,"Setting brightness level to: %d",global_brightness);
  debugmsg(msg);
  FastLED.setBrightness(global_brightness);
}



void debugmsg(char *foo)
{
  char logmsg[MAX_MSG_SIZE];
  char upcstr[10];
  ltoa(calc_uptime(), upcstr, 10);
  sprintf(logmsg,"[%s ut(%s)]: %s",WiFi.localIP().toString().c_str(), upcstr,foo);

  Serial.println(logmsg);
  if (WiFi.localIP().toString() != "0.0.0.0")
  {
    UDP.beginPacket("10.65.3.241", 1225);
    UDP.write(logmsg);
    UDP.endPacket();
  }
}
void debugmsg(const char *foo)
{
  // I only want to maintain the one function above, so this is hacky...
  char msg[MAX_MSG_SIZE];
  sprintf(msg,"%s", foo);
  debugmsg(msg);
}
