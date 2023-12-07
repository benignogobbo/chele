// +---------------------------------------------------+
// | Dew Point sniffers pump currents monitor          |
// | Benigno Gobbo                                     |
// | 2023-12-06                                        |
// | V0.2.0                                            |
// | © 2023 Istituto Nazionale di Fisica Nucleare      |
// +---------------------------------------------------+

//#define SERIAL_RX_BUFFER_SIZE 128

#include <Wire.h>
#include <Adafruit_GFX.h>      // From https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SH110X.h>   // From https://github.com/adafruit/Adafruit_SH110x

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SH1106G display = Adafruit_SH1106G( SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET );

#include "changelog.h"
#include "config.h"
#include "network.h"
#include "current.h"
#include "utils.h"

Network net;
Current curr;
Config cfg;

void setup() {

  Serial.begin( 9600 );
  while( !Serial );
  #if DEBUG
    Serial1.begin( 9600 );
  #endif
  
  display.begin( 0x3C, true );
  display.setContrast( 0x00 );
  display.clearDisplay();
  display.setTextSize( 1 );
  display.setTextColor( SH110X_WHITE, SH110X_BLACK );
  display.setCursor( 0, 0 );
  display.println( "Initializing..." );
  display.display();
  DBPRINTLN();
  DBPRINTLN( "Initialization started..." );
  delay(50);

  cfg.init();
  cfg.ropen();
  cfg.decode();
  cfg.close();

  Wire.setClock(400000UL);

  curr.init();
  
  net.init();
  server.begin();
  
  delay( 500 );
  
  net.ntpInit();
  String date = net.timeString( timeClient.getEpochTime() );

  Serial.println( "ready" );

  long currT = millis();
  while( ( millis() - currT ) < 1000 ); // Wait a second...
  
  DBPRINTLN( "Check serial" );
  if( Serial.available() ) {
    String cmd = Serial.readString();
    if( cmd.indexOf( "sdupd" ) != -1 ) {
      DBPRINTLN( "Trying to update SD" );
      cfg.updateSD();
    }
  }
  DBPRINTLN( "Now starting data readout..." );
  display.println( "Now starting..." );
  display.display();
  
}

void loop() {
  
  timeClient.update();
  String date = net.timeString( timeClient.getEpochTime() );
  String currentsR = date;
  String currentsM = date;
  String currentsRM = date;
  String Irms6[4], Imos6[4], Irms3[4], Imos3[4];
  for( int i=0; i<4; i++ ) {
    double ir = curr.getCurrentRMS( i );
    double im = curr.getCurrentMOS( i );
    Irms6[i] = String( ir, 6 );
    Imos6[i] = String( im, 6 );
    Irms3[i] = String( 1000. * ir, 0 );
    Imos3[i] = String( 1000. * im, 0 );
    currentsR  += " " + Irms6[i];
    currentsM  += " " + Imos6[i];
    currentsRM += " " + Irms6[i] + " " + Imos6[i];
  }

  display.clearDisplay();
  display.setCursor(0,0);
  display.drawFastHLine( 0, 1, 100, 1 );
  display.setCursor( 2, 4 );
  display.println( date );
  display.drawFastHLine( 0, 13, 100, 1 );
  display.setCursor( 0, 15 );
  for( int i=0; i<4; i++ ) {
    display.println( "I" + String( i ) + ": " + Irms3[i] + "(" + Imos3[i] + ")" + " mA" );
  }
  display.display();

  DBPRINTLN( "R. " + currentsR );
  DBPRINTLN( "M. " + currentsM );

  EthernetClient client = server.available();
  if( client ) {
    DBPRINTLN( "Got a client request..." );
    byte c = 1, i = 0;
    while( c != 255 && i < 256 ) {
      c = client.read();
      ethBuff[i] = char( c );
      i++;
    }
    ethBuff[i-1] = '\0';
    if( strncmp( ethBuff, "data", 4 ) == 0 ) {
      server.write( currentsRM.c_str() );
    }
    else if( strncmp( ethBuff, "vers", 4 ) == 0 ) {
      String version = String(VERSION_MAJOR) + "." + String(VERSION_MINOR) + "." + String(VERSION_REV);
      server.write( version.c_str() );
    }
    else if( strncmp( ethBuff, "reset", 5 ) == 0 ) {
      resetAVR();
    }
    else if( strncmp( ethBuff, "dispon", 6 ) == 0 ) {
      display.oled_command( SH110X_DISPLAYON ); 
    }
    else if( strncmp( ethBuff, "dispoff", 7 ) == 0 ) {
      display.oled_command( SH110X_DISPLAYOFF ); 
    }
  }
  delay( 100 );
}
