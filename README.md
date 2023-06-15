# chele
This is the software for the Arduino based pump current meter developed to monitor the KEK Belle II experiment dew point sniffers.
It consists of the Arduino firmware (chele-x.y.z-yyyymmdd), tested on (original) Arduino Mega 2560 and the Linux client C++ sample tested on a PC running Ubuntu 22.04.
Communication between Linux Client and arduino is via TCP/IP socket connection

## Commands
Arduino understands the following client requests:
| command | action |
|---------|---------|
| `data` | Returns current measurements |
| `vers` | Returns the software version |
| `reset` | Performs a software AVR reset |
| `dispon` | Turns OLED display on |
| `dispoff` | Turns OLED display off |




