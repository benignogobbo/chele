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

## Data format

## config.txt
Contains the network configuration and NTP server address needed for Arduino board network access setup.
The format is the following (X = hexadecinal byte, N decimal byte):

    mac X:X:X:X:X:X
    ip N.N.N.N
    gw N.N.N.N
    dns N.N.N.N
    nm N.N.N.N
    ntp N.N.N.N

Example
    mac DE:AD:FA:CE:00:00
    ip 192.168.69.2
    gw 192.168.69.1
    dns 1.1.1.1
    nm 255.255.255.0
    ntp 192.168.69.1


