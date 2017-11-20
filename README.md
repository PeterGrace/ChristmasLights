ChristmasLights
===============

Intro
-----
My oldest son was saying he loves christmas lights.  We've never put up christmas lights before, so I figured if I was going to put up any kind of lights, I should probably put up lights that I can control dynamically, so that I can light up for any season.


Implementation
--------------
This code runs on multiple Adafruit HUZZAH esp8266 breakout boards.  Each board controls one or more strips in my installation.

It is:
 - Field upgradable
 - Dynamically changeable via JSON POST
 - A labor of love

How to use it
-------------
1. Clone this repo
2. Create an auth.h file in the directory with the .ino file, that contains #define statements for ssid and password for your wifi.
3. Upload the code to your esp8266 board.
4. Plug your strip into the board.  You need a led strip that is compatible with FastLED -- changing this code for your strip is beyond the scope of this readme.   My WS2812b strip I plugged the signal line into port 5.
5. Power the strip and the esp8266.  The HUZZAH support up to 6V on V+, so I power the HUZZAH and the LED lights via the same +5V, 60A power supply I use for central distribution.
6. Send JSON payloads to http://boardip/lights to control the different modes.

Configurations available
------------------------
mode:
  - 1: Red, Green, Orange, Blue random pulsing
  - 2: icicle mode
  - 3: confetti mode
  - 4: rainbow (solid)
  - 5: rainbow (beats by bpm)
  - 6: Red-White-Blue Chase (for 4th of july?)
  - 10: red pulse
  - 11: constant red
  - 20: green pulse
  - 21: constant green
  - 30: blue pulse
  - 31: constant blue
  - 40: orange pulse
  - 41: constant orange
  - 50: swipe (red and green)
  - 51: swipe (red and blue)
  - 52: swipe (blue and green)
  - 99: Gradient Mode

bpm:
  - numeric value that defines how many beats per minute the animation should use.  Keep in mind that this value is not capped at a high value, it is possible to annoy the watchdog timer in an esp8266 microcontroller by setting this really high.

brightness:
 - numeric value between 0 and 255.  255 is very bright, 0 is off.  Middle value is a combination of those two states.

icicle_new_min, icicle_new_max:
  - These two values are the times, in milliseconds, that the program should use as the lower and upper bound of random generation of time.  New icicles will drip every new_min to new_max milliseconds.

icicle_wait_min, icicle_wait_max:
  - currently not implemented, but I plan to have the drip of water occasionally pause, as I've seen on some commercial icicle strips.

Example
-------
```curl -XPOST -d'{"mode":5,"bpm":8,"brightness":4}' http://yourip/lights```

This command will set mode to rainbow pulse, with the color swiping at 8 beats per minute, with a very low brightness.



