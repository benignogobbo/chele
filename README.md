# chele
This is the software for the Arduino based pump current meter developed to monitor the KEK Belle II experiment dew point sniffers.
It consists of the Arduino firmware (chele-x.y.z-yyyymmdd), tested on (original) Arduino Mega 2560 and the Linux client C++ sample tested on a PC running Ubuntu 22.04.
Communication between Linux Client and arduino is via TCP/IP socket connection

## NOTEs on Release 0.4.0 and newer
The program stops ADC readout activities on any request from network. If the request is "data" then it retuns the previous readout currents. As a complete currents evaluation takes about 4s, if the network requests frequency is faster than every 5s, the arduino will never be able to estimate currents.
Now the software supports a 128x64 pixels OLED yellow monochrome display and a 320x240 pixels TFT color display.

## Build info
The following additional libraries are needed to build the code (they all can be installed using Arduino IDE Library Manager). The numbers in round brackets are the release numbers used to build the software).<br>

These are needed to drive the OLED monochrome display:<br>
Adafruit GFX Library   (1.11.9)<br>
Adafruit SH110x        (2.1.10)<br><br>

And these are needed to drive ILI9341 320x240 TFT display:<br>
Adafruit ILI9341       (1.6.0)<br>
U8g2_for_adafruit_GFX  (1.8.0)<br><br>

This is needed to read the ADCs: <br>
Adafruit ADS1X15        (2.5.0)<br><br>

These libs are needed for networking: <br>
Ethernet                (2.0.2)<br>
NTPClient               (3.2.1)<br>
Time                    (1.6.1)<br><br>

## Commands
Arduino understands the following client requests:
| command | action |
|---------|---------|
| `data` | Returns current measurements |
| `vers` | Returns the software version |
| `reset` | Performs a software AVR reset |
| `dispon` | Turns OLED/TFT display on |
| `dispoff` | Turns OLED/TFT display off |

## Data format
<pre> dateString timeString currR0 currM0 currR1 currM1 currR2 currM2 currR3 currM3 </pre>
Where:<br>
dateString is the readout date in yyyy-mm-dd format <br>
timeString is the radout time in hh:mm:ss format <br>
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
The client software folder contains some comunication examples between a Linux machine and the Arduino board as well as a tool to download via TPC/IP an updated config.txt file to the Arduino SD card.
| program | action |
|---------|---------|
| `test <ip>` | Asks and dumps arduino measurements  |
| `reset <ip>` | Asks Arduino to perform a software reset |
| `dispon <ip>` | Asks Arduino to turn the display on |
| `dispoff <ip>` | Asks Arduino to turn the display off |
| `command <ip> <command>` | Asks Arduino to perform a generic &lt;command&gt; |
| `updatesd <s/n>` | Tries to send a condig.txt file to Ardino via serial connection (fragile). <s/n> is the Arduino Shield serial number |




