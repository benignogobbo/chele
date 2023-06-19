// +--------------------------------------------------------+
// | Benigno Gobbo - INFN Trieste                           |
// | V0.1 20230518                                          |
// +--------------------------------------------------------+

#include <iostream>
#include <iomanip>
#include <exception>
#include <string>
#include <sstream>
#include <unistd.h>
#include <getopt.h>

#include "cheleEthClient.h"

// <><><><><><> Main...
int main( int argc, char *argv[] ) {

  const std::string  ipAddress = "192.168.69.2";
  const int          tcpipPort = 57778; 
  const unsigned int sleepSecs = 5;
  const std::string  command = "data";
  
  std::cout << "\033c";
  
  try {

    CheleEthClient client;
    bool status = client.ethernetConnect(  ipAddress, tcpipPort );
    std::cout << "Connection status: " << status << std::endl;
    
    while( true ) {
      client.ethernetWrite( command );
      usleep( 300000 );
      std::string s = client.ethernetRead();
      std::stringstream ss( s );
      std::string date, time;
      ss >> date; 
      ss >> time;
      float currR[4], currM[4];
      for( int i=0; i<4; i++ ) { ss >> currR[i]; ss >> currM[i]; }
      std::cout << date << " " << time;
      std::cout.setf( std::ios::fixed, std::ios::floatfield );
      for( int i=0; i<4; i++ ) std::cout << "    I" << i << " = " << std::setprecision(6) << currR[i] << " " << currM[i];
      std::cout.unsetf( std::ios::floatfield );
      std::cout << std::endl;
      sleep( sleepSecs );
    }
  } catch( std::string error ) {
    std::cout << "\033[31mError: " << error << "\033[0m" << std::endl;
    return 1;
  }

  return 0;
  
}
