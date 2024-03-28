// +---------------------------------------------------+
// | Dew Point sniffers pump currents monitor          |
// | Benigno Gobbo                                     |
// | 2024-03-28                                        |
// | V0.3.4                                            |
// | © 2024 Istituto Nazionale di Fisica Nucleare      |
// +---------------------------------------------------+

// #define SERIAL_RX_BUFFER_SIZE 128
#include <Wire.h>
#include <Adafruit_GFX.h>           // See https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SH110X.h>        // See https://github.com/adafruit/Adafruit_SH110x
#include <Adafruit_ILI9341.h>       // See https://github.com/adafruit/Adafruit_ILI9341
#include <U8g2_for_Adafruit_GFX.h>  // See https://github.com/olikraus/U8g2_for_Adafruit_GFX

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SH1106G dispOled = Adafruit_SH1106G( SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET );

#define TFT_RESET 46
#define TFT_LITE  47
#define TFT_DC    48
#define TFT_CS    49
Adafruit_ILI9341 dispTft = Adafruit_ILI9341( TFT_CS, TFT_DC );
U8G2_FOR_ADAFRUIT_GFX u8g2;

#include "changelog.h"
#include "config.h"
#include "network.h"
#include "current.h"
#include "utils.h"

Network net;
static constexpr auto SOCKET_ERROR = int{-1}; // TCP client errors
static constexpr auto ETH_BUFFER_SIZE = size_t{256};

Current curr;
Config cfg;

String currentsR;
String currentsM;
String currentsRM;

// +------------------------------------------------------------------------------+
// | The standard Arduino setup function...                                       |
// +------------------------------------------------------------------------------+
void setup(){

    Serial.begin( 9600 );
    while( !Serial );
    #if DEBUG
      Serial1.begin( 9600 );
    #endif

    dispOled.begin( 0x3C, true );
    dispOled.setContrast( 0x00 );
    dispOled.clearDisplay();
    dispOled.setTextSize( 1 );
    dispOled.setTextColor( SH110X_WHITE, SH110X_BLACK );
    dispOled.setCursor( 0, 0 );
    dispOled.println( "Initializing..." );
    dispOled.display();
    
    DBPRINTLN();
    DBPRINTLN("Initialization started...");
    
    cfg.init();
    cfg.ropen();
    cfg.decode();
    cfg.close();

    pinMode( TFT_LITE, OUTPUT );
    digitalWrite( TFT_LITE, HIGH );
    pinMode( TFT_RESET, OUTPUT );
    digitalWrite( TFT_RESET, HIGH );

    dispTft.begin();
    dispTft.setRotation( 1 );
    dispTft.fillScreen( ILI9341_BLACK );
    u8g2.begin( dispTft );  
    u8g2.setFontMode( 0 );
    u8g2.setFontDirection( 0 );
    u8g2.setFont(  u8g2_font_helvB24_tf );
    u8g2.setCursor( 0, 30 );
    u8g2.setForegroundColor( ILI9341_YELLOW );
    u8g2.println( "Initializing..." );

    Wire.setClock( 400000UL );

    curr.init();

    net.init();
    server.begin();

    delay(500);

    net.ntpInit();
    String date = net.timeString( timeClient.getEpochTime() );

    Serial.println( "ready" );

    long currT = millis();
    while((millis() - currT) < 1000 ); // Wait a second...

    DBPRINTLN( "Check serial" );
    if( Serial.available() ){
        String cmd = Serial.readString();
        if( cmd.indexOf( "sdupd" ) != -1 ) {
            DBPRINTLN( "Trying to update SD" );
            cfg.updateSD();
        }
    }
    DBPRINTLN( "Now starting data readout..." );
    dispOled.println( "Now starting..." );
    dispOled.display();

    dispTft.fillScreen( ILI9341_BLACK );
    u8g2.setFont(  u8g2_font_helvB24_tf );
    u8g2.setCursor( 0, 30 );
    u8g2.setForegroundColor( ILI9341_GREEN );
    u8g2.println( "Now starting..." );
}

// +------------------------------------------------------------------------------+
// | The standard Arduino loop function...                                        |
// +------------------------------------------------------------------------------+
void loop() {
    String Irms6[4], Imos6[4], Irms4[4], Imos4[4];
    bool gotCurrents = true;
    for( int i = 0; i < 4; i++ ) {
        double ir = 0, im = 0;
        bool gotIt = gotCurrents = curr.getCurrent(i, ir, im);
        if( gotIt ) {
            Irms6[i] = String( ir, 6 );
            Imos6[i] = String( im, 6 );
            Irms4[i] = String( ir, 4 );
            Imos4[i] = String( im, 4 );
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
            currentsR += " " + Irms6[i];
            currentsM += " " + Imos6[i];
            currentsRM += " " + Irms6[i] + " " + Imos6[i];
        }

        dispOled.clearDisplay();
        dispOled.setCursor( 0, 0 );
        dispOled.drawFastHLine( 0, 1, 128, 1 );
        dispOled.setCursor( 8, 4 );
        dispOled.println( date );
        dispOled.drawFastHLine( 0, 13, 128, 1 );
        dispOled.setCursor( 0, 15 );
        for( int i = 0; i < 4; i++ ){
            dispOled.println( "I" + String(i) + ": " + Irms4[i] + "(" + Imos4[i] + ")" + " A" );
        }
        dispOled.display();

        dispTft.fillScreen( ILI9341_BLACK );      
        dispTft.drawLine( 0, 1, 320, 1, ILI9341_RED );
        dispTft.drawLine( 0, 2, 320, 2, ILI9341_RED );
        u8g2.setFont(  u8g2_font_luRS18_tr );
        u8g2.setForegroundColor( ILI9341_GREEN );
        u8g2.setCursor( 30, 26 );
        u8g2.println( date );
        dispTft.drawLine( 0, dispTft.getCursorY()+30, 320, dispTft.getCursorY()+30, ILI9341_RED );
        dispTft.drawLine( 0, dispTft.getCursorY()+31, 320, dispTft.getCursorY()+31, ILI9341_RED );
        u8g2.setCursor( 0, u8g2.getCursorY()+10 );
        for( int i=0; i<4; i++ ) {
             u8g2.setForegroundColor( ILI9341_YELLOW );
             u8g2.setCursor( 0, u8g2.getCursorY()+4 );
             u8g2.print( "I" + String( i ) + ": " );
             u8g2.setForegroundColor( ILI9341_WHITE );
             u8g2.setCursor( 50, u8g2.getCursorY()+4 );
             u8g2.print( Irms4[i] );
             u8g2.setCursor( 160, u8g2.getCursorY()+4 );
             u8g2.println( "(" + Imos4[i] + ")" + " A" );
        }
        DBPRINTLN( "R. " + currentsR );
        DBPRINTLN( "M. " + currentsM );
    }

    EthernetClient client = server.available();
    if( client ) {
        DBPRINTLN( "Got a client request..." );
        auto i = uint16_t{0};
        char ethBuff[ETH_BUFFER_SIZE];
        for(auto c = int{0}; ( (i + 1) < ETH_BUFFER_SIZE) && (c != SOCKET_ERROR); ++i ) {
            c = client.read();
            ethBuff[i] = static_cast<char>(c);
        }
        ethBuff[i - 1] = '\0';

        if( strncmp( ethBuff, "data", 4 ) == 0 ) {
            client.write( currentsRM.c_str() );
        }
        else if( strncmp( ethBuff, "vers", 4 ) == 0 ){
            String version = String(VERSION_MAJOR) + "." + String(VERSION_MINOR) + "." + String(VERSION_REV);
            client.write( version.c_str() );
        }
        else if( strncmp(ethBuff, "reset", 5 ) == 0 ){
            resetAVR();
        }
        else if( strncmp(ethBuff, "dispon", 6 ) == 0 ){
            dispOled.oled_command( SH110X_DISPLAYON );
            digitalWrite( TFT_LITE, HIGH );
        }
        else if( strncmp(ethBuff, "dispoff", 7 ) == 0 ){
            dispOled.oled_command( SH110X_DISPLAYOFF );
            digitalWrite( TFT_LITE, LOW );
        }
    }
    delay(100);
}
