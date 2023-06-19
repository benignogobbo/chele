// +--------------------------------------------------------+
// | Benigno Gobbo - INFN Trieste                           |
// | V0.1 20230519                                          |
// +--------------------------------------------------------+

#include <iostream>
#include <iomanip>
#include <exception>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <getopt.h>

#include "cheleEthClient.h"

// <><><><>><> Usage
void usage( char* cp ) {
  std::cout << std::endl << "usage: " << cp << "command" << std::endl << std::endl;
}


// <><><><><><> Main...
int main( int argc, char *argv[] ) {    
  
  const std::string ipAddress = "192.168.69.2";
  const int         tcpipPort = 57778; 
  std::string command, vers;
  char *cp;

  if(( cp = (char*) strrchr( argv[0], '/' )) != NULL ) {
    ++cp;
  }
  else {
    cp = argv[0];
  }

  if( argc == 2 ) {
    command = argv[1];
  }
  else {
    usage( cp );
    return -1;
  }
  
  //std::cout << "\033c";
  
  try {

    CheleEthClient client;
    bool status = client.ethernetConnect(  ipAddress, tcpipPort );
    std::cout << "Connection status: " << status << std::endl;

    if( command == "vers" ) {
      client.ethernetWrite( command );
      usleep( 500000 );
      vers = client.ethernetRead();
      std::cout << "Version: " << vers << std::endl;
    }
    else {
    client.ethernetWrite( command );
      usleep( 300000 );
    }  

  } catch( std::string error ) {
    std::cout << "\033[31mError: " << error << "\033[0m" << std::endl;
    return 1;
  }

  return 0;
  
}
