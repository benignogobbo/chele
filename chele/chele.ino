// +---------------------------------------------------+
// | Dew Point sniffers pump currents monitor          |
// | Benigno Gobbo                                     |
// | 2024-05-24                                        |
// | V0.4.0                                            |
// | © 2024 Istituto Nazionale di Fisica Nucleare      |
// +---------------------------------------------------+

#include <Adafruit_GFX.h>          // See https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_ILI9341.h>      // See https://github.com/adafruit/Adafruit_ILI9341
#include <Adafruit_SH110X.h>       // See https://github.com/adafruit/Adafruit_SH110x
#include <U8g2_for_Adafruit_GFX.h> // See https://github.com/olikraus/U8g2_for_Adafruit_GFX
#include <TouchScreen.h>           // See https://github.com/adafruit/Adafruit_TouchScreen/
#include <Wire.h>
#include "displays.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

// TFT Arduino Mega 
#define TFT_RESET 46
#define TFT_LITE  47
#define TFT_DC    48
#define TFT_CS    49
// TFT_MISO       50
// TFT_MOSI       51
// TFT_SCK        52

#define XP A12
#define XM A13
#define YP A14
#define YM A15
#define OFF_AFTER (300UL)
uint32_t time0;

Adafruit_SH1106G dispOled = Adafruit_SH1106G( SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET );

Adafruit_ILI9341 dispTft = Adafruit_ILI9341( TFT_CS, TFT_DC );
U8G2_FOR_ADAFRUIT_GFX u8g2; 

TouchScreen touchs = TouchScreen( XP, YP, XM, YM, 300 ); 

#include "config.h"
#include "changelog.h"
#include "current.h"
#include "network.h"
#include "utils.h"
#include "trst.h"

Network net;
static constexpr auto SOCKET_ERROR = int{-1}; // TCP client errors
static constexpr auto ETH_BUFFER_SIZE = size_t{256};

Current curr;
Config cfg;

String currentsR;
String currentsM;
String currentsRM;

// +------------------------------------------------------------------------------+
// | The standard Arduino setup function... |
// +------------------------------------------------------------------------------+
void setup() { 

  Serial.begin( 9600 ); 
# if DEBUG
  Serial1.begin( 9600 );
# endif


  delay( 1000 );
  cfg.init();
  cfg.ropen();
  cfg.decode();
  cfg.close(); 

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
  }

  DBPRINTLN();
  DBPRINTLN( "Initialization started..." );

  pinMode( TFT_LITE, OUTPUT );
  digitalWrite( TFT_LITE, HIGH ) ;
  pinMode( TFT_RESET, OUTPUT );
  digitalWrite( TFT_RESET, HIGH );

  if( useTftd ) {
    dispTft.begin();
    dispTft.setRotation(1);
    dispTft.fillScreen( ILI9341_BLACK );
    u8g2.begin( dispTft );
    u8g2.setFontMode( 0 );
    u8g2.setFontDirection( 0 );
    dispTft.drawRGBBitmap( (dispTft.width()-TRST_WIDTH)/2, (dispTft.height()-TRST_HEIGHT)/2, trst, TRST_WIDTH, TRST_HEIGHT );
  }
  
  delay(2000);

  if( useOled ) {
    dispOled.setTextColor( SH110X_WHITE, SH110X_BLACK );
    dispOled.setCursor( 0, 0 );
    dispOled.println( "Initializing..." );
    dispOled.display();
  } 

  if( useTftd ) {
    dispTft.fillScreen( ILI9341_BLACK );
    u8g2.setCursor( 30, 30 );
    u8g2.setForegroundColor( ILI9341_YELLOW );
    //u8g2.setFont( u8g2_font_helvB24_tf );
    u8g2.setFont( u8g2_font_helvB18_tf );
    u8g2.println( "Initializing Net & Time:" );
  } 

  Wire.setClock( 400000UL );

  curr.init();

  net.init();
  server.begin();

  delay(500);

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

  if( useOled ) {  
    dispOled.println( "Now starting..." );
    dispOled.display();
  }

  if( useTftd ) {
    dispTft.fillScreen( ILI9341_BLACK );
    u8g2.setFont( u8g2_font_helvB24_tf );
    u8g2.setCursor( 30, 120 );
    u8g2.setForegroundColor( ILI9341_GREEN );
    u8g2.println( "Now starting..." );
  }
  
  time0 = millis();
}

// +------------------------------------------------------------------------------+
// | The standard Arduino loop function... |
// +------------------------------------------------------------------------------+
void loop() {
  double Irms[4], Imos[4];
  bool gotCurrents = true;
  for( int i = 0; i < 4; i++ ) {
    double ir = 0, im = 0;
    bool gotIt = gotCurrents = curr.getCurrent( i, ir, im );
    if( gotIt ) {
      Irms[i] = ir;
      Imos[i] = im;
    } 
    else {
      gotCurrents = false;
    }
  }

  if( gotCurrents ) {
    timeClient.update();
    String date = net.timeString( timeClient.getEpochTime() );
    currentsR = date;
    currentsM = date;
    currentsRM = date;
    for( int i = 0; i < 4; i++ ) {
      currentsR += " " + String( Irms[i], 6 );
      currentsM += " " + String( Imos[i], 6 );
      currentsRM += " " + String( Irms[i], 6 ) + " " + String( Imos[i], 6 );
    }

    if( useOled ) {
      dispOled.clearDisplay();
      dispOled.setCursor( 0, 0 );
      dispOled.drawFastHLine( 0, 1, 128, 1 );
      dispOled.setCursor( 8, 4 );
      dispOled.println( date );
      dispOled.drawFastHLine( 0, 13, 128, 1 );
      dispOled.setCursor( 0, 15 );
      for( int i = 0; i < 4; i++ ) {
        dispOled.println( "I" + String(i) + ": " + String( Irms[i], 4 ) + "(" + String( Imos[i], 4 ) + ")" + " A" );
      }
      dispOled.display();
    }

    if( useTftd ) {
      dispTft.fillScreen( ILI9341_BLACK );
      dispTft.drawLine( 0, 1, 320, 1, ILI9341_CYAN );
      dispTft.drawLine( 0, 2, 320, 2, ILI9341_CYAN );
      u8g2.setFont( u8g2_font_luRS18_tr );
      u8g2.setForegroundColor( ILI9341_CYAN );
      u8g2.setCursor( 30, 26 );
      u8g2.println( date );
      dispTft.drawLine( 0, dispTft.getCursorY() + 30, 320, dispTft.getCursorY() + 30, ILI9341_CYAN );
      dispTft.drawLine( 0, dispTft.getCursorY() + 31, 320, dispTft.getCursorY() + 31, ILI9341_CYAN );
      u8g2.setCursor( 0, u8g2.getCursorY() + 15 );
      for( int i = 0; i < 4; i++ ) {
        u8g2.setForegroundColor( ILI9341_YELLOW );
        u8g2.setCursor( 30, u8g2.getCursorY() + 30 );
        u8g2.print( "I" + String(i) + ": " );
        if( Imos[i] > 3 ) {
          u8g2.setForegroundColor(ILI9341_ORANGE);
        } 
        else {
          u8g2.setForegroundColor( ILI9341_GREEN );
        }
        u8g2.setCursor( 80, u8g2.getCursorY() );
        u8g2.print( String( Irms[i], 4 ) );
        u8g2.setCursor( 190, u8g2.getCursorY() );
        u8g2.print( "(" + String(Imos[i]) + ")" + " A" );
      }
    }

    DBPRINTLN( "R. " + currentsR );
    DBPRINTLN( "M. " + currentsM );
  }

  EthernetClient client = server.available();
  if( client ) {
    DBPRINTLN( "Got a client request..." );
    auto i = uint16_t{0};
    char ethBuff[ ETH_BUFFER_SIZE ];
    for( auto c = int{0}; ( (i + 1) < ETH_BUFFER_SIZE ) && ( c != SOCKET_ERROR ); ++i ) {
      c = client.read();
      ethBuff[i] = static_cast<char>(c);
    }
    ethBuff[i - 1] = '\0';

    if( strncmp( ethBuff, "data", 4 ) == 0 ) {
      client.write( currentsRM.c_str() );
    } 
    else if ( strncmp(ethBuff, "vers", 4 ) == 0 ) {
      String version = String( VERSION_MAJOR ) + "." + String( VERSION_MINOR ) + "." + String( VERSION_REV );
      client.write( version.c_str() );
    } 
    else if( strncmp( ethBuff, "reset", 5 ) == 0 ) {
      resetAVR();
    } 
    else if( strncmp( ethBuff, "dispon", 6 ) == 0 ) {
      if( useOled ) dispOled.oled_command( SH110X_DISPLAYON );
      if( useTftd ) {
        DBPRINTLN( "Display on (Command)..." );
        digitalWrite( TFT_LITE, HIGH );
      }
      time0 = millis();
    } 
    else if( strncmp( ethBuff, "dispoff", 7 ) == 0 ) {
      if( useOled ) dispOled.oled_command( SH110X_DISPLAYOFF );
      if( useTftd ) {
        DBPRINTLN( "Display off (Command)..." );
        digitalWrite( TFT_LITE, LOW );
      }
    }
  }

  if( ( millis() - time0 ) > 1000UL*OFF_AFTER ) { 
    if( useOled ) dispOled.oled_command( SH110X_DISPLAYOFF );
    if( useTftd ) {
      digitalWrite( TFT_LITE, LOW );
      DBPRINTLN( "Display off (Timer)..." );
    }
    time0 = millis();
  }
  TSPoint p = touchs.getPoint();
  if( p.z > touchs.pressureThreshhold ) {
    if( useOled ) dispOled.oled_command( SH110X_DISPLAYON );
    if( useTftd ) {
      DBPRINTLN( "Display on (Touch)..." );
      digitalWrite( TFT_LITE, HIGH );
    }
  }
  delay(100);
}
