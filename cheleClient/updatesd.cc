// +--------------------------------------+
// | Update chele SD via serial           |
// | Benigno Gobbo - INFN Trieste         |
// | V0.1 20230608                        |
// +--------------------------------------+

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <typeinfo>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <exception>

#include <unistd.h>
#include <sys/types.h>
// serial...
#include <fcntl.h>
#include <dirent.h>
#include <termios.h>

// <><><><><><> Local things
std::string    _devManu;
std::string    _devSerial;
int            _fd;
double         _readTimeout;
char           _buff[256];
struct termios _oldtio;
struct termios _newtio;

// <><><><><><> Scale readout timeout
const float _readTimeout0 = 0.2;
void scaleReadTimeout( float scale ) {
  _readTimeout = _readTimeout0 * scale;
  return;
}

// <><><><><><> Reset readout timeout
void resetReadTimeout( void ) {
  _readTimeout = _readTimeout0;
  return;
}


// <><><><><><> try to write some characters to serial
void serialWrite( std::string data ) {
  int status = write( _fd, data.c_str(), data.size() );
  if( status != data.size() ) {
    throw( std::string( strerror( errno ) ) );
    return;
  }
}


// <><><><><><> try to read something from seriar (slow but safer...)
std::string serialRead() {

  fd_set read_fds, write_fds, except_fds;
  FD_ZERO( &read_fds );
  FD_ZERO( &write_fds );
  FD_ZERO( &except_fds );
  FD_SET( _fd, &read_fds );
  struct timeval timeout;

  timeout.tv_usec = modf( _readTimeout, &_readTimeout ) * 1000000; 
  timeout.tv_sec  = _readTimeout;
  resetReadTimeout();
  
  std::string s = "";
  char buff[512];
  int status = 0;
  int rv = 0;
  do {
    if( ( rv = select( _fd+1, &read_fds, &write_fds, &except_fds, &timeout )) ==1 ) {
      status = read( _fd, buff, 1 );
      if( status == 1 ) {
	s += buff[0];
      }
    }
  } while( status > 0 && rv == 1 );

  s[s.length()-1] = '\0';
  return(s);
}

// <><><><><><> try to read something from serial, fast but unsafe
std::string serialFastRead() {
  char buff[512];
  int status = read( _fd, buff, sizeof(buff) );
  if( status < 0 ) {
    throw( std::string( strerror( errno ) ) );
    return( std::string( "" ) );
  }
  else {
    return( std::string( buff ) );
  }
}

// <><><><><><> Connect to Serial
void serialConnect( std::string manuName, std::string prodSN  ) {

  _devManu   = manuName;
  _devSerial = prodSN;

  std::cout << "\033c\033[4mLooking for " << _devManu << " (s/n "
	    << _devSerial << ") module... \033[0m" << std::endl;
  char buff[256];
  std::string prod = "\"" + _devManu + "\"";
  std::string seri = "\"" + _devSerial + "\"";
  std::string comm1 = "udevadm info -a -n ";
  std::string comm2 = " | xargs ";
  std::string comm3 = " | grep ";

  // Look for device:
  std::vector<std::string> files;
  DIR *dp = opendir( "/dev" );
  struct dirent *dirp;
  while( (dirp = readdir(dp))  != NULL ) {
    files.push_back( std::string( dirp->d_name ) );
  }
  std::string s1 = "ttyUSB";
  std::string s2 = "ttyACM";
  std::vector<std::string> devices;

  for( unsigned int i=0; i<files.size(); i++ ) {
    if( files[i].substr(0,6) == s1 || files[i].substr(0,6) == s2 ) {
      devices.push_back( "/dev/" + files[i] );
    }
  }

  std::vector<std::string> foundDev;
  for( unsigned int i=0; i<devices.size(); i++ ) {
    bool found = false;
    std::string command = comm1 + devices[i] + comm2 + comm3 + prod + comm3 + seri;
    FILE* f = popen( command.c_str(), "r" );
    if( f ) {
      while( !feof( f ) ) {
        if( fgets( buff, 256, f ) != NULL && !found ) {
	  found = true;
          foundDev.push_back( devices[i] );
        }
      }
      pclose( f );
    }
  }
  
  if( foundDev.size() != 1 ) {
    std::cout << "\033[31m Found too few or many devices. I need just one. Exit...\033[0m" << std::endl;
    exit(1);
  }
  
  _fd = open( foundDev[0].c_str(), O_RDWR | O_NOCTTY );
  if( _fd < 0 ) {
    throw( std::string( strerror( errno ) ) );
    return;
  }

  tcgetattr( _fd, &_oldtio );
  _newtio = _oldtio;

  cfsetispeed( &_newtio, (speed_t)B9600 );
  cfsetospeed( &_newtio, (speed_t)B9600 );
  //cfmakeraw(   &_newtio );

  _newtio.c_cflag &= ~PARENB;
  _newtio.c_cflag &= ~CSTOPB;
  _newtio.c_cflag &= ~CSIZE;
  _newtio.c_cflag |= CS8;
  _newtio.c_lflag |= ICANON;
  
  tcsetattr( _fd, TCSANOW, &_newtio );
  tcflush( _fd, TCIOFLUSH );

  std::cout << "Wait for Arduino initialisation..." << std::endl;
  int count = 0; bool gotIt = false;
  while( !gotIt && count < 50 ) {
    std::cout << "\033[2K\033[33m" << std::fixed << std::setprecision(1) << float(count)/2. << " seconds\033[0m" << std::endl;
    std::cout << std::resetiosflags( std::ios::fixed ) << std::setprecision(6);
    count+=1;
    std::string s = serialRead();
    if( s.substr(0,5) == "ready" ) {
      gotIt = true;
    }
    usleep(500000);
    std::cout << "\033[2A" << std::endl;
  }
  if( gotIt ) {
    std::cout << "Got connection..." << std::endl;
    usleep(100000);
  }
  else {
    std::cout << "\033[31mError: Arduino does not comunicate...\033[0m"  << std::endl;      
    exit(1);
  }  
  
  return;
}

// <><><><><><> Restore default Serial Settings and close it
void serialDisconnect( void ) {

  tcsetattr( _fd, TCSANOW, &_oldtio ); /* restore old port settings */
  tcflush( _fd, TCIOFLUSH );
  if( close( _fd ) < 0 ) {
    throw ( std::string( strerror( errno ) ) );
  } 
  return;
}

// <><><><><><> Disconnect and Connect again to Serial
void serialReconnect( std::string prodName, std::string prodSN ) {
  serialDisconnect();
  serialConnect(  prodName, prodSN );
  return;
}

// <><><><><><> The main...
int main( int argc, char **argv ) {

  if( argc != 2 ) {
    std::cout << "The board serial number is a mandatory argument of this program!" << std::endl;
    return -1;
  }
  
  std::string manufacturer = "Arduino";
  std::string serialNumber( argv[1] );
  
  std::ifstream f;
  f.open( "config.txt" );
  if( f.fail() ) {
    std::cout << "Oh dear, I cannot find config.txt input file" << std::endl;
    return 1;
  }

  try {
  
    serialConnect( manufacturer, serialNumber );
    serialWrite( "sdupd" );
    
    std::string toSend = "", line = "";
    while( std::getline( f, line ) ) {
      toSend +="," + line.substr(0,line.size()-1);
    }
    toSend = toSend.substr( 1 );
    f.close();

    bool ack = false;
    for( int i=0; i<100 && !ack; i++ ) {
      std::string answ = serialRead();
      if( answ.substr( 0, 7 ) == "waiting" ) {
	ack = true;
      }
    }
    if( ack ) {
      usleep( 100000 );
      serialWrite( toSend.substr(0, toSend.find("nm")) );
      usleep( 100000 );
    }
    ack = false;
    for( int i=0; i<100 && !ack; i++ ) {
      std::string answ = serialRead();
      if( answ.substr( 0, 7 ) == "waiting" ) {
	ack = true;
      }
    }
    if( ack ) {
      usleep( 100000 );
      serialWrite( toSend.substr(toSend.find("nm")) );
      usleep( 100000 );
    }
    serialDisconnect();

  } catch( std::string s ) {
    std::cout << s << std::endl;
    return 1;
  }
  
  return 0;
}
