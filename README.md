# Volvo V50 RTI + Crankshaft + Raspberry Pi + Arduino = ❤️

Run Android Auto on Volvo V50 RTI screen by retrofitting it with Raspberry Pi & [Crankshaft](https://getcrankshaft.com/).

[![Working example](media/android_auto.jpg)](https://photos.app.goo.gl/vtM3ymQ5z1pJNeDw8)

[Photos and videos »](https://photos.app.goo.gl/vtM3ymQ5z1pJNeDw8)

## Features

- Steering wheel controls
- Auto-open display when Android phone is connected to USB

Missing / TODO:

- Automatic brightness adjustment using light sensor
- Sound output to the car stereo via RPi
- Microphone

## Ingredients

- Volvo V50
- Raspberry Pi 3B
- Arduino Pro Micro
- 6.5" AT065TN14 800x480 LCD + driver board
- MCP2004 LIN bus transceiver (or any compatible like MCP2003, MCP2025)
- 12V -> 5V step down converter, like XL4005 or LM2596. I would choose 5amps instead of 3amps.
- HDMI slim flat cable
- USB extension cable

## Overview

Raspberry Pi is running [Crankshaft](https://getcrankshaft.com/) which enables Android Auto support.

Arduino is listening for steering wheel button events via LIN bus transceiver. It converts these events to keyboard / mouse HID events and sends to Raspberry Pi via USB.

Via the same USB cable Raspberry Pi is sending phone state to Arduino.

Arduino is sending serial events to Volvo RTI screen module and controls opening / closing of the screen.

Original RTI screen is replaced with AT065TN14 and connected to RPi using HDMI cable. Driver board is powered by 12V from RTI screen module.

RPi is powered from step down converter which converts 12V from the car to 5V.

## Configure Arduino

Build and upload [volvo_crankshaft.ino](volvo_crankshaft.ino) to Arduino.

## Configure RPi / Crankshaft

Copy files from [crankshaft/](crankshaft/) to RPi.

Enable serial service

> systemctl enable volvo_crankshaft

> systemctl start volvo_crankshaft 

## Links

- [Volvo V50 LIN bus reader](https://github.com/laurynas/volvo_linbus) - reading steering wheel controls
- [Volvo V50 RTI screen with Arduino controls](https://github.com/laurynas/volvo) - sending serial + VGA signals to original RTI screen
- [Volvo V50 RTI screen removal](https://www.youtube.com/watch?v=MJirMelq5ys)
- [volvo V50 waterfall panel/radio removal](https://www.youtube.com/watch?v=Xo5NpBt04qs)
