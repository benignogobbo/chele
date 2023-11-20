# chele
This is the software for the Arduino based pump current meter developed to monitor the KEK Belle II experiment dew point sniffers.
It consists of the Arduino firmware (chele-x.y.z-yyyymmdd), tested on (original) Arduino Mega 2560 and the Linux client C++ sample tested on a PC running Ubuntu 22.04.
Communication between Linux Client and arduino is via TCP/IP socket connection

## Build info
The following additional libraries are needed to build the code.<br>

These are needed to drive the OLED monochrome display:<br><br>
Adafruit GFX Library  1.11.9 or newer<br>
Adafruit SH110x       4.0.1 or newer<br><br>

This is needed to read the ADCs: <br>
https://github.com/adafruit/Adafruit_ADS1X15   2.4.2 or newer<br>

Those libs are needed for networking: <br><br>
Ethernet        2.0.2 or newer
NTPClient       3.2.1 or newer

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
<pre> dateString timeString currR0 currM0 currR1 currM1 currR2 currM2 currR3 currM3 </pre>
Where:<br>
dateString is the readout date in yyyy-mm-dd format <br>
timeString is the radout time in hh:mm format <br>
currRn (n: 0...3) is the readout RMS current of the n-th line <br>
currMn (n: 0...3) is the readount Max/sqrt(2) current of the n-th line <br>

## config.txt
Contains the network configuration and NTP server address needed for Arduino board network access setup. this file must be copied onto a Micro SD card to be inserted in the extension board of the Arduino.
Currently the dns is configured but not used.
The format is the following (X: an hexadecinal byte, N: a decimal byte):

<pre>mac X:X:X:X:X:X
ip N.N.N.N
gw N.N.N.N
dns N.N.N.N
nm N.N.N.N
ntp N.N.N.N </pre>

Example:

    mac DE:AD:FA:CE:00.00
    ip 192.168.69.2
    gw 192.168.69.1
    dns 1.1.1.1
    nm 255.255.255.0
    ntp 192.168.69.1

## Client software
The client software folder contains some examples comunication examples between a Linux machine and the Arduino board as well as a tool to download via TPC/IP an updated config.txt file to the Arduino SD card.
| program | action |
|---------|---------|
| `test <ip>` | Asks and dumps arduino measurements  |
| `reset <ip>` | Asks Arduino to perform a software reset |
| `dispon <ip>` | Asks Arduino to turn the display on |
| `dispoff <ip>` | Asks Arduino to turn the display off |
| `command <ip> <command>` | Asks Arduino to perform a generic &lt;command&gt; |
| `updatesd <s/n>` | Tries to send a condig.txt file to Ardino via serial connection (fragile). <s/n> is the Arduino Shield serial number |




