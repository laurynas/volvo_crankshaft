# Volvo V50 RTI + Crankshaft + Raspberry Pi + Arduino = ❤️

Run Android Auto on Volvo V50 RTI screen by retrofitting it with Raspberry Pi & [Crankshaft](https://getcrankshaft.com/).

[![Working example](media/android_auto.jpg)](https://photos.app.goo.gl/vtM3ymQ5z1pJNeDw8)

[Photos and videos »](https://photos.app.goo.gl/vtM3ymQ5z1pJNeDw8)

## Features

- Steering wheel controls
- Open/close display when Android phone is plugged/unplugged

Missing / TODO:

- Automatic brightness adjustment using light sensor
- Sound output to the car stereo via RPi
- Microphone

I'm playing audio via Bluetooh audio adapter. 

## Ingredients

- Volvo V50
- Raspberry Pi 3B or later
- Arduino Pro Micro
- 6.5" AT065TN14 800x480 LCD + driver board. AT065TN14 exactly matches original RTI display dimentions (width x height). Also I found some alternatives - [thinner screen](https://www.aliexpress.com/item/4000329488912.html?spm=a2g0s.9042311.0.0.439c4c4dvR4vq8) and [smaller driver board with audio output](https://www.aliexpress.com/item/4001175095149.html?spm=a2g0s.9042311.0.0.439c4c4dvR4vq8).
- MCP2004 LIN bus transceiver. Or compatible like MCP2003, MCP2025.
- 12V -> 5V step down converter, like XL4005 or LM2596. I would choose 5amps instead of 3amps.
- [HDMI slim flat FPC cable](https://amzn.to/3hjNZCM)
- USB extension cable
- Micro USB to USB cable

## Overview

This is just a rough scheme showing how main components are connected. 

![Rough scheme](media/sketch_bb.png)

Raspberry Pi is running [Crankshaft](https://getcrankshaft.com/) which enables Android Auto support.

Arduino is listening for steering wheel button events via [LIN bus](https://github.com/laurynas/volvo_linbus) transceiver. It converts these events to keyboard / mouse HID events and sends to Raspberry Pi via USB. Make sure you are using Arduino [hardware supported by the HID library](https://github.com/NicoHood/HID).

Via the same USB cable Raspberry Pi is sending phone state to Arduino.

Arduino is sending [serial events](https://github.com/laurynas/volvo#screen-control-signal) to Volvo RTI screen module and controls opening / closing of the screen.

Original RTI screen is replaced with AT065TN14 and connected to RPi using HDMI cable. Driver board is powered by 12V from RTI screen module.

RPi is powered from step down converter which converts 12V from the car to 5V.

## Configure Arduino

Build and upload [volvo_crankshaft.ino](volvo_crankshaft.ino) to Arduino.

## Configure RPi / Crankshaft

1. Connect to Crankshaft via [SSH](https://github.com/opencardev/crankshaft/wiki/Crankshaft-dev-mode).
2. Unlock readonly Crankshaft filesystem using `csmt` command line utility.
3. Copy files from [crankshaft/](crankshaft/) to RPi using SSH or just create them via the terminal. Place files under appropriate system folders and make `/opt/service_volvo_crankshaft.sh` executable.
4. Enable the serial service:

> systemctl enable volvo_crankshaft

> systemctl start volvo_crankshaft 

### Configure Crankshaft

Configure Crankshaft using Crankshaft UI -> Settings.

1. Enable keyboard controls - Arduino works as "keyboard" sending keyboard events to RPi.
2. Disable audio if you are playing audio via bluetooth adapter.

## Controls

1. When Android phone is connected, RTI screen should pop automatically and streering wheel controls can be used to navigate Android auto.
2. Clicking "Enter" on the steering wheel buttons when Android phone is disconneted will pop RTI screen and joystic will work as a mouse to navigate Crankshaft UI (you can use it to go to the settings). 
3. Long press of "Back" button will put the screen down.

## Tips

- MMM module is still receiving SWM events and occasionally might speak someting on your audio system :) To avoid that, you can disconnect it, but you need to put [optical bypass](https://allegro.pl/oferta/petla-swiatlowod-most-kabel-audi-seat-skoda-zestaw-9688273368) because otherwise radio wouldn't work. 

## Alternative - Using original RTI LCD display

I have also succeeded using original LCD display too. [More details in the wiki »](https://github.com/laurynas/volvo_crankshaft/wiki/Using-original-RTI-LCD-display).

## Links

- [Volvo V50 LIN bus reader](https://github.com/laurynas/volvo_linbus) - reading steering wheel controls
- [Volvo V50 RTI screen with Arduino controls](https://github.com/laurynas/volvo) - sending serial + VGA signals to original RTI screen
- [Volvo V50 RTI screen removal](https://www.youtube.com/watch?v=MJirMelq5ys)
- [Volvo V50 waterfall panel/radio removal](https://www.youtube.com/watch?v=Xo5NpBt04qs)
- [Volvo V50 glove box removal](https://www.youtube.com/watch?v=bLI-HLhO0_c) - if you decide to remove MMM module
- [Luuk Android Auto RTI project](https://luuk.cc/p/vD2f/Android_Auto_on_Volvo_RTI) - for Volvo S60, S80, V70, etc.
