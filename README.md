# Volvo V50 RTI + Crankshaft + Raspberry Pi + Arduino = ❤️

Work in progress...

### Configure Arduino

Build and upload [volvo_crankshaft.ino](volvo_crankshaft.ino) to Arduino Pro Micro.

### Configure RPi / Crankshaft

Copy files from [crankshaft/](crankshaft/) to RPi.

Enable serial service

> systemctl enable volvo_crankshaft

> systemctl start volvo_crankshaft 

### Connect wires

1. Connect Arduino to RPi using USB cable.


### Links

- [Volvo V50 LIN bus reader](https://github.com/laurynas/volvo_linbus) - reading steering wheel controls
- [Volvo V50 RTI screen with Arduino controls](https://github.com/laurynas/volvo) - sending serial + VGA signals to original RTI screen
