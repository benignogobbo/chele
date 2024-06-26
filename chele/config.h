// +---------------------------------------------------+
// | Read Configuration from SD                        |
// | Benigno Gobbo                                     |
// | © 2024 Istituto Nazionale di Fisica Nucleare      |
// +---------------------------------------------------+
#ifndef CONFIG_H
#define CONFIG_H

#include "displays.h"
#include "utils.h"
#include <SD.h>
#include <SPI.h>

#define DEBUG 0

#if DEBUG
#define DBPRINT(...) Serial1.print(__VA_ARGS__)
#define DBPRINTLN(...) Serial1.println(__VA_ARGS__)
#else
#define DBPRINT(...)
#define DBPRINTLN(...)
#endif

uint8_t _mac[6], _ip[4], _gw[4], _ns[4], _nm[4];
char _ntp[32];
const int SDSS = 4;

const String filename = "config.txt";

// +------------------------------------------------------------------------------+
// | Config. It reads net configuration from SD card |
// +------------------------------------------------------------------------------+
class Config {
 public:
  void init( void ) {
    pinMode( SDSS, OUTPUT );
    digitalWrite( SDSS, HIGH );
    if( !SD.begin(SDSS) ) {
      DBPRINTLN( "SD device initialization failed." );
      if( useOled ) { 
        dispOled.begin( 0x3C, true );
        dispOled.setContrast( 0x00 );
        dispOled.clearDisplay();
        dispOled.setTextSize(1);
        dispOled.setTextColor( SH110X_WHITE, SH110X_BLACK );
        dispOled.setCursor( 0, 30 );
        dispOled.println( "      (C) 2024" );
        dispOled.print(   "    INFN Trieste" );
        dispOled.display();
        dispOled.println( "SD init failed" );
        dispOled.display();
      } 
      if( useTftd ) {
        pinMode( TFT_LITE, OUTPUT );
        digitalWrite( TFT_LITE, HIGH ) ;
        pinMode( TFT_RESET, OUTPUT );
        digitalWrite( TFT_RESET, HIGH );
        dispTft.begin();
        dispTft.setRotation(1);
        dispTft.fillScreen( ILI9341_BLACK );
        u8g2.begin( dispTft );
        u8g2.setFontMode( 0 );
        u8g2.setFontDirection( 0 );
        u8g2.setCursor( 30, 120 );
        u8g2.setForegroundColor( ILI9341_RED );
        //u8g2.setFont( u8g2_font_helvB24_tf );
        u8g2.setFont( u8g2_font_helvB18_tf );
        u8g2.println( "SD initialization failed" );
      }
      while (true);
    }
  }

  void ropen(void) { _fd = SD.open( filename.c_str() ); }

  void wopen(void) { _fd = SD.open( filename.c_str(), FILE_WRITE | O_TRUNC ); }

  void decode(void) {
    String line, strs[5];
    int strCount = 0;
    while( _fd.available() ) {
      line = _fd.readStringUntil('\n');
      if( line[0] != '#' ) { // '#' starts a comment line
        strCount = 0;
        while( line.length() > 0 ) {
          int index = line.indexOf(' ');
          if( index == -1 ) {
            strs[strCount++] = line;
            break;
          } 
          else {
            strs[strCount++] = line.substring( 0, index );
            line = line.substring( index + 1 );
          }
        }

        if( strCount == 2 ) {
          if(strs[0] == "mac" ) {
            // sscanf( strs[1].c_str(), "%x:%x:%x:%x:%x:%x", &_mac[0], &_mac[1],
            // &_mac[2], &_mac[3], &_mac[4], &_mac[5] );
            int id = 0;
            while( strs[1].length() > 0 ) {
              int index = strs[1].indexOf(':');
              if( index == -1 ) {
                _mac[id++] = strtol( strs[1].c_str(), NULL, 16 );
                break;
              } 
              else {
                _mac[id++] = strtol(strs[1].substring( 0, index).c_str(), NULL, 16 );
                strs[1] = strs[1].substring( index + 1 );
              }
            }
          } 
          else if( strs[0] == "ip" ) {
            int id = 0;
            while( strs[1].length() > 0 ) {
              int index = strs[1].indexOf('.');
              if ( index == -1 ) {
                _ip[id++] = strs[1].toInt();
                break;
              } 
              else {
                _ip[id++] = strs[1].substring( 0, index ).toInt();
                strs[1] = strs[1].substring( index + 1 );
              }
            }
          } 
          else if( strs[0] == "gw" ) {
            int id = 0;
            while( strs[1].length() > 0 ) {
              int index = strs[1].indexOf('.');
              if( index == -1 ) {
                _gw[id++] = strs[1].toInt();
                break;
              } 
              else {
                _gw[id++] = strs[1].substring( 0, index ).toInt();
                strs[1] = strs[1].substring( index + 1 );
              }
            }
          } 
          else if( strs[0] == "dns" ) {
            int id = 0;
            while( strs[1].length() > 0 ) {
              int index = strs[1].indexOf('.');
              if( index == -1 ) {
                _ns[id++] = strs[1].toInt();
                break;
              } 
              else {
                _ns[id++] = strs[1].substring( 0, index ).toInt();
                strs[1] = strs[1].substring( index + 1 );
              }
            }
          } 
          else if( strs[0] == "nm" ) {
            int id = 0;
            while( strs[1].length() > 0 ) {
              int index = strs[1].indexOf('.');
              if( index == -1 ) {
                _nm[id++] = strs[1].toInt();
                break;
              } 
              else {
                _nm[id++] = strs[1].substring( 0, index ).toInt();
                strs[1] = strs[1].substring( index + 1 );
              }
            }
          } 
          else if( strs[0] == "ntp" ) {
            strncpy( _ntp, strs[1].c_str(), 32 );
            for( int i = 0; i < 32; i++ ) if (_ntp[i] == '\r') _ntp[i] = 0;
          }
        }
      }
    }
    DBPRINTLN( "Settings from SD card: " );
    DBPRINT( String("mac: ") );
    for( int j = 0; j < 5; j++ ) {
      DBPRINT( String( _mac[j], HEX ));
      DBPRINT( ":" );
    };
    DBPRINTLN( String( _mac[5], HEX ));
    DBPRINT( String( "ip:  " ));
    for(int j = 0; j < 3; j++) DBPRINT( String( _ip[j] ) + String( "." ));
    DBPRINTLN( String(_ip[3]) );
    DBPRINT( String("gw:  ") );
    for( int j = 0; j < 3; j++ ) DBPRINT( String( _gw[j] ) + String( "." ));
    DBPRINTLN( String( _gw[3] ));
    DBPRINT( String( "dns: " ));
    for( int j = 0; j < 3; j++ ) DBPRINT( String( _ns[j] ) + String( "." ));
    DBPRINTLN( String( _ns[3] ));
    DBPRINT( String( "nm:  " ));
    for( int j = 0; j < 3; j++ ) DBPRINT( String( _nm[j] ) + String( "." ));
    DBPRINTLN( String( _nm[3] ));
    DBPRINTLN( String( "ntp: " ) + String( _ntp ));
  }

  void close(void) { _fd.close(); }

  void updateSD() {
    wopen();
    Serial.println( "waiting" );
    Serial.flush();
    delay( 100 );
    String s = Serial.readString();
    s = s.substring( s.indexOf("mac") );
    Serial.println( "waiting" );
    Serial.flush();
    delay(100);
    String s1 = Serial.readString();
    s1 = s1.substring( s1.indexOf("nm") );
    s = s + s1;
    DBPRINTLN( "Got string: " + s );
    int index = 0;
    do {
      index = s.indexOf(",");
      String line = s.substring( 0, index );
      s = s.substring( index + 1 );
      _fd.println( line );
    } while( index != -1 );
    close();
    // reboot the Arduino to load changes...
    resetAVR();
  }

 private:
  File _fd;
};

#endif // CONFIG_H
