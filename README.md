# NeoPixelPusher (NPP)
Control software for Adafruit NeoPixels.

* Simple serial protocol
* Effect Mode:
  * 58 built-in effects
  * 42 built-in color patterns
  * Custom effects possible, up to 96 frames on 4 channels
  * Custom color patterns possible, up to 32 colors
  * Variable speed control
  * Can randomly change modes periodically
* XLM (Explorateria Light Machine) Mode:
  * Simulates an exhibit at a certain San Francisco science museum
  * Perfect for NeoPixel Ring 60
* Clock Mode:
  * 3 display modes (point, line, binary)
  * Customizable colors for hours, minutes, seconds
  * Perfect for NeoPixel Ring 60
* Colors and fades are gamma-corrected
* Brightness adjustment
* Settings saved to EEPROM

## Default Configuration

* 60 NeoPixels on Pin 6
* "Factory reset" on Pin 5 (connect to ground to erase EEPROM on startup)
* RTC on SCL/SDA
* Can be modified at top of .ino file

## Serial Protocol

* `E` - Switch to Effect Mode
  * `ER` - Report whether randomization is in effect and time between effects
    * `ER0` - Do not randomly change effects
    * `ER1` - Randomly change effects
    * `ER` _ms_ - Set time between changing effects
  * `EM` - Report current effect (0-57, or 254 if custom)
    * `EM` _n_ - Set current effect
  * `ECP` - Report current color pattern preset (0-41, or 254 if custom)
    * `ECP` _n_ - Set current color pattern preset
  * `ECS` _rrggbb_ ... - Set custom color pattern (up to 4 colors initially)
  * `ECX` _rrggbb_ ... - Extend custom color pattern (up to 4 colors at a time, up to 32 at max)
  * `ECT` - Report number of colors
    * `ECT` _n_ - Report value of _n_-th color
  * `EBP` - Report current blink pattern present (0-17, or 254 if custom)
    * `EBP` _n_ - Set current blink pattern preset
  * `EBS` _ms_ _n_ _aabbccdd_ ... - Set custom effect / blink pattern
    * _ms_ - Milliseconds per frame, 2 hex digits
    * _n_ - Number of channels, 1 digit (1-4)
    * _aabbccdd_ - Brightness levels per channel, (2 * number of channels) hex digits
    * Up to 3 - 12 frames initially, depending on number of channels
  * `EBX` _aabbccdd_ ... - Extend custom effect / blink pattern
    * _aabbccdd_ - Brightness levels per channel, (2 * number of channels) hex digits
    * Up to 3 - 14 frames at a time, depending on number of channels
    * Up to 96 frames at max
  * `EBT` - Report milliseconds per frame, number of channels, and number of frames
    * `EBT` _n_ - Report brightness levels of _n_-th frame
  * `ES` - Report speed adjustment (0.0 to ~255.996 in increments of ~0.004)
    * `ES` _x_ - Set speed adjustment
* `X` - Switch to XLM (Explorateria Light Machine) Mode
    * `XN0+` - increase number on for first channel
    * `XN0-` - decrease number on for first channel
    * `XN1+` - increase number on for second channel
    * `XN1-` - decrease number on for second channel
    * `XO0+` - increase number off for first channel
    * `XO0-` - decrease number off for first channel
    * `XO1+` - increase number off for second channel
    * `XO1-` - decrease number off for second channel
    * `XS0+` - increase speed of first channel
    * `XS0-` - decrease speed of first channel
    * `XS1+` - increase speed of second channel
    * `XS1-` - decrease speed of second channel
    * `XV0` - reverse first channel
    * `XV1` - reverse second channel
    * `XR0` - start / resume first channel
    * `XR1` - start / resume second channel
    * `XF0` - stop / freeze first channel
    * `XF1` - stop / freeze second channel
    * `XZ0` - reset first channel to default
    * `XZ1` - reset second channel to default
* `C` - Switch to Clock Mode
  * `CCH` _rrggbb_ - Set hour color
  * `CCM` _rrggbb_ - Set minute color
  * `CCS` _rrggbb_ - Set second color
  * `CM` _n_ - Set clock mode
    * `CM0` - Point mode
    * `CM1` - Line mode
    * `CM2` - Binary mode
  * `CS` _yyyymmdd_ _hhmmss_ - Set clock time
  * `CT` - Report clock time
* `PB` _n_ - Set brightness level (0-255)
* `PZ` - Report free RAM

## Effect Modes

* 0 - Steady On\
  ![](wiki/steady.gif)
* 1 - Scroll Left\
  ![](wiki/scrollleft.gif)
* 2 - Scroll Right\
  ![](wiki/scrollright.gif)
* 3 - Blink\
  ![](wiki/blink.gif)
* 4 - Cycle\
  ![](wiki/cycle.gif)
* 5 - Cycle On/Off\
  ![](wiki/cycleonoff.gif)
* 6 - Color Wipe\
  ![](wiki/colorwipe.gif)
* 7 - Color Wipe On/Off\
  ![](wiki/colorwipeonoff.gif)
* 8 - Fast Fade In/Out\
  ![](wiki/fastfadeinout.gif)
* 9 - Fast Fade Cycle\
  ![](wiki/fastfadecycle.gif)
* 10 - Fast Fade Cycle In/Out\
  ![](wiki/fastfadecycleinout.gif)
* 11 - Slow Fade In/Out\
  ![](wiki/slowfadeinout.gif)
* 12 - Slow Fade Cycle\
  ![](wiki/slowfadecycle.gif)
* 13 - Slow Fade Cycle In/Out\
  ![](wiki/slowfadecycleinout.gif)
* 14 - Walk Left\
  ![](wiki/walkleft.gif)
* 15 - Walk Right\
  ![](wiki/walkright.gif)
* 16 - Walk Left/Right\
  ![](wiki/walkleftright.gif)
* 17 - Move Left\
  ![](wiki/hopleft.gif)
* 18 - Move Right\
  ![](wiki/hopright.gif)
* 19 - Oscillate\
  ![](wiki/oscillate.gif)
* 20 - Running Lights A Left\
  ![](wiki/runninglightsaleft.gif)
* 21 - Running Lights A Right\
  ![](wiki/runninglightsaright.gif)
* 22 - Running Lights B Left\
  ![](wiki/runninglightsbleft.gif)
* 23 - Running Lights B Right\
  ![](wiki/runninglightsbright.gif)
* 24 - Twinkle
* 25 - Random Twinkle
* 26 - Endless Twinkle
* 27 - Moodlight
* 28 - Sparkle
* 29 - Random Sparkle
* 30 - Snow Sparkle
* 31 - Static
* 32 - Tail Chase Left
* 33 - Tail Chase Right
* 34 - Larson Scanner A
* 35 - Larson Scanner B
* 36 - Bouncing Balls A
* 37 - Bouncing Balls B
* 38 - Fire
* 39 - Meteor Rain
* 40 - Chase 1/2\
  ![](wiki/chase12.gif)
* 41 - Chase 1/3\
  ![](wiki/chase13.gif)
* 42 - Chase 1/4\
  ![](wiki/chase14.gif)
* 43 - Chase 2/3\
  ![](wiki/chase23.gif)
* 44 - Chase 2/4\
  ![](wiki/chase24.gif)
* 45 - Chase 3/4\
  ![](wiki/chase34.gif)
* 46 - Slo Glo 3\
  ![](wiki/sloglo3.gif)
* 47 - Slo Glo 4\
  ![](wiki/sloglo4.gif)
* 48 - Waves 3\
  ![](wiki/waves3.gif)
* 49 - Waves 4\
  ![](wiki/waves4.gif)
* 50 - Twinkle Flash A\
  ![](wiki/twinkleflasha.gif)
* 51 - Twinkle Flash B\
  ![](wiki/twinkleflashb.gif)
* 52 - Twinkle Flash C\
  ![](wiki/twinkleflashc.gif)
* 53 - Chasing Flash\
  ![](wiki/chasingflash.gif)
* 54 - Walk 3\
  ![](wiki/walk3.gif)
* 55 - Walk 4\
  ![](wiki/walk4.gif)
* 56 - Oscillate 3\
  ![](wiki/oscillate3.gif)
* 57 - Oscillate 4\
  ![](wiki/oscillate4.gif)

## Color Patterns

* 0 - Bright White\
  ![](wiki/crystal.png)
* 1 - Warm White\
  ![](wiki/warmwhite.png)
* 2 - Azure\
  ![](wiki/icicle.png)
* 3 - Multicolor Mini (Red, Blue, Green, Magenta, Gold)\
  ![](wiki/multicolormini.png)
* 4 - Multicolor Solid (Red, Blue, White, Orange, Green)\
  ![](wiki/multicolorsolid.png)
* 5 - Multicolor Translucent (Red, Blue, Green, Yellow, Magenta)\
  ![](wiki/multicolortranslucent.png)
* 6 - Rainbow (Red, Orange, Yellow, Green, Cyan, Blue, Violet, Magenta)\
  ![](wiki/rainbow.png)
* 7 - Pastel Rainbow\
  ![](wiki/pastel.png)
* 8 - Red & White\
  ![](wiki/candycane.png)
* 9 - Red & Green\
  ![](wiki/noel.png)
* 10 - Blue & White\
  ![](wiki/cabana.png)
* 11 - Blue & Green\
  ![](wiki/surfandturf.png)
* 12 - Autumn Colors (Brown, Red, Orange, Yellow)\
  ![](wiki/autumn.png)
* 13 - Orange\
  ![](wiki/pumpkin.png)
* 14 - Summer Colors (Yellows)\
  ![](wiki/summer.png)
* 15 - Red White & Blue\
  ![](wiki/independence.png)
* 16 - Spring Colors (Yellows & Greens)\
  ![](wiki/spring.png)
* 17 - Valentines (Reds & Pinks)\
  ![](wiki/truelove.png)
* 18 - Winter Colors (Cyans & Blues)\
  ![](wiki/winter.png)
* 19 - Purples\
  ![](wiki/tammyday.png)
* 20 - Rainbow Deluxe\
  ![](wiki/rainbowdeluxe.png)
* 21 - Assorted Pastels\
  ![](wiki/beckiescolors.png)
* 22 - Red, Yellow, Blue, Green\
  ![](wiki/primaries.png)
* 23 - Orange, Magenta, Yellow\
  ![](wiki/powellstation.png)
* 24 - Magenta, Yellow\
  ![](wiki/festivebowtree.png)
* 25 - Red\
  ![](wiki/redalert.png)
* 26 - Gold\
  ![](wiki/underconstruction.png)
* 27 - Green\
  ![](wiki/clover.png)
* 28 - Blue\
  ![](wiki/bluelightspecial.png)
* 29 - Red & Blue\
  ![](wiki/lawandorder.png)
* 30 - Barber Pole (Red, White, Blue, White)\
  ![](wiki/barberpole.png)
* 31 - Red White & Green\
  ![](wiki/genova.png)
* 32 - Assorted Colors 1\
  ![](wiki/mosswood1.png)
* 33 - Assorted Colors 2\
  ![](wiki/mosswood2.png)
* 34 - Purple, Indigo, Magenta\
  ![](wiki/magic.png)
* 35 - Pink, Magenta, Cyan\
  ![](wiki/laughter.png)
* 36 - Red, Orange, Yellow, Green, Blue, Violet\
  ![](wiki/loyalty.png)
* 37 - Orange, Yellow, Green\
  ![](wiki/honesty.png)
* 38 - White, Violet, Azure\
  ![](wiki/generosity.png)
* 39 - Yellow, Pink, Aquamarine\
  ![](wiki/kindness.png)
* 40 - Pastel Pink, Orange, Yellow, White, Cyan, Purple\
  ![](wiki/harmony.png)
* 41 - Cyan, Azure, Purple, Pink, Orange, Yellow, Green\
  ![](wiki/unowen.png)
