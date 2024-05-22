// +---------------------------------------------------+
// | Network                                           |
// | Benigno Gobbo                                     |
// | © 2024 Istituto Nazionale di Fisica Nucleare      |
// +---------------------------------------------------+
#ifndef NETWORK_H
#define NETWORK_H

#include <Ethernet.h>
#include <EthernetUdp.h> // Needed by NTP
#include <NTPClient.h>
#include <SPI.h>
#include <TimeLib.h> // From https://github.com/PaulStoffregen/Time
#include "displays.h"

constexpr auto EthernetSS = 10;
constexpr auto MegaHardwareSS = 53;

constexpr auto _TCPIPCommunicationPort = 57778;
constexpr auto _UDPCommunicationPort = 8888;

constexpr auto _TimeZone = +9; // JST
constexpr auto _UpdateInterval = 60000;


// Ethernet Setup, TPCIP
EthernetServer server = EthernetServer( _TCPIPCommunicationPort );
// NTP
EthernetUDP udp;
NTPClient timeClient( udp );

//+--------------------------------------------------------------------------+
//| Network                                                                  |
//+--------------------------------------------------------------------------+
class Network {
 public:
  Network( void ){};
  void init( void );
  void ntpInit( void );
  String timeString( time_t );

 private:
  String _twoDigits( int );
};

void Network::ntpInit( void ) {
  udp.begin( _UDPCommunicationPort );
  timeClient.setPoolServerName( _ntp );
  timeClient.setUpdateInterval( _UpdateInterval );
  timeClient.setTimeOffset( 3600 * _TimeZone );
  timeClient.begin();
  timeClient.forceUpdate();
}

//+--------------------------------------------------------------------------+
String Network::_twoDigits( int number ) {
  String s;
  if( number < 10 ) {
    s = '0' + String( number );
  } 
  else {
    s = String( number );
  }
  return( s );
}

//+--------------------------------------------------------------------------+
String Network::timeString( time_t tse ) {
  String timedate = String( year( tse ) );
  timedate += "-" + _twoDigits( month( tse ));
  timedate += "-" + _twoDigits( day( tse ));
  timedate += " " + _twoDigits( hour( tse ));
  timedate += ":" + _twoDigits( minute( tse ));
  timedate += ":" + _twoDigits( second( tse ));
  return( timedate );
}

//+--------------------------------------------------------------------------+
void Network::init( void ) {

  pinMode( EthernetSS, OUTPUT );
  // pinMode( MegaHardwareSS, OUTPUT );
  digitalWrite( EthernetSS, HIGH );
  Ethernet.init( EthernetSS );

  IPAddress ip(_ip[0], _ip[1], _ip[2], _ip[3]);
  IPAddress gateway(_gw[0], _gw[1], _gw[2], _gw[3]);
  IPAddress subnet(_nm[0], _nm[1], _nm[2], _nm[3]);
  IPAddress dns(_ns[0], _ns[1], _ns[2], _ns[3]);

  int count = 0;
  do {
    Ethernet.begin( _mac, ip, dns, gateway, subnet );
    delay( 100 );
  } while( count++ < 5 && ( Ethernet.linkStatus() != LinkON ) );

  DBPRINT( "Local IP address     : " );
  DBPRINTLN( Ethernet.localIP() );
  DBPRINT( "Gateway IP address   : " );
  DBPRINTLN( Ethernet.gatewayIP() );
  DBPRINT( "Network subnet mask  : " );
  DBPRINTLN( Ethernet.subnetMask() );
  DBPRINT( "DNS IP address       : " );
  DBPRINTLN( Ethernet.dnsServerIP() );
  DBPRINT( "NTP server           : " );
  DBPRINTLN( String( _ntp ) );

  if( useOled ) {
    dispOled.print( "IP : " );
    dispOled.println( Ethernet.localIP() );
    dispOled.print( "GW : " );
    dispOled.println( Ethernet.gatewayIP() );
    dispOled.print( "SM : " );
    dispOled.println( Ethernet.subnetMask() );
    dispOled.print( "DNS: " );
    dispOled.println( Ethernet.dnsServerIP() );
    dispOled.print( "NTP: " );
    dispOled.println( String( _ntp ) );
    dispOled.display();
  }

  if( useTftd ) {
    u8g2.setFont( u8g2_font_helvB18_tf );
    u8g2.setCursor( 30, u8g2.getCursorY() + 30 );
    u8g2.setForegroundColor( ILI9341_CYAN );
    u8g2.print( "IP : " );
    u8g2.print( Ethernet.localIP() );
    u8g2.setCursor( 30, u8g2.getCursorY() + 30 );
    u8g2.print( "GW : " );
    u8g2.print( Ethernet.gatewayIP() );
    u8g2.setCursor( 30, u8g2.getCursorY() + 30 );
    u8g2.print( "SM : " );
    u8g2.print( Ethernet.subnetMask() );
    u8g2.setCursor( 30, u8g2.getCursorY() + 30 );
    u8g2.print( "DNS: " );
    u8g2.print( Ethernet.dnsServerIP() );
    u8g2.setCursor( 30, u8g2.getCursorY() + 30 );
    u8g2.print( "NTP: " );
    u8g2.print( String(_ntp) );
  }

  if( Ethernet.hardwareStatus() == EthernetNoHardware ) {
    DBPRINTLN( String(count) + "Error in ethernet initialization" );
  } 
  else if( Ethernet.hardwareStatus() == EthernetW5500 ) {
    if(Ethernet.linkStatus() == Unknown ) {
      DBPRINTLN( "Error ethernet link unknown" );
    } 
    else if( Ethernet.linkStatus() == LinkOFF ) {
      DBPRINTLN( "Error in ethernet link off" );
    } 
    else if( Ethernet.linkStatus() == LinkON ) {
      DBPRINTLN( "Ethernet link on" );
    }
  }
}

#endif // NETWORK_H
