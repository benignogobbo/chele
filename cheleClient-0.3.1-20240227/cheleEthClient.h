// +-------------------------------------+
// | Ethernet class for the chele client |
// | Benigno Gobbo - INFN Trieste        |
// | V0.1 20230421                       |
// +-------------------------------------+

#ifndef CHELEETHCLIENT_H
#define CHELEETHCLIENT_H

#include <string>
#include <netinet/in.h>
#include <netdb.h>

class CheleEthClient {

 public:
  CheleEthClient() {};
  bool        ethernetConnect( std::string hostAddress , int portNo );
  std::string ethernetRead( void );
  int         ethernetWrite( std::string s );
  void        ethernetDisconnect( void );
  bool        ethernetReconnect( std::string hostAddress, int portNo ); 
  
 private:
  int                _port;
  int                _socketFd;
  struct sockaddr_in _serverAddress;
  struct hostent*    _server;
  char               _buff[256];
  };

#endif //CHELEETHCLIENT_H
