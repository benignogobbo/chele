// +--------------------------------------------------------+
// | Benigno Gobbo - INFN Trieste                           |
// | V0.2 2023-06-22                                        |
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
  std::cout << std::endl << "usage: " << cp << " box_ip_address command" << std::endl << std::endl;
}


// <><><><><><> Main...
int main( int argc, char *argv[] ) {    
  
  std::string ipAddress;
  const int         tcpipPort = 57778; 
  std::string command, vers;
  char *cp;

  if(( cp = (char*) strrchr( argv[0], '/' )) != NULL ) {
    ++cp;
  }
  else {
    cp = argv[0];
  }

  if( argc == 3 ) {
   ipAddress = argv[1];
   command = argv[2];
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
