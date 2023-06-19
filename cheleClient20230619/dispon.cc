// +--------------------------------------------------------+
// | Benigno Gobbo - INFN Trieste                           |
// | V0.1 20230519                                          |
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

  const std::string ipAddress = "192.168.69.2";
  const int         tcpipPort = 57778; 
  const std::string command = "dispon";
  
  std::cout << "\033c";
  
  try {

    CheleEthClient client;
    bool status = client.ethernetConnect(  ipAddress, tcpipPort );
    std::cout << "Connection status: " << status << std::endl;
    
    client.ethernetWrite( command );
    usleep( 300000 );

  } catch( std::string error ) {
    std::cout << "\033[31mError: " << error << "\033[0m" << std::endl;
    return 1;
  }

  return 0;
  
}
