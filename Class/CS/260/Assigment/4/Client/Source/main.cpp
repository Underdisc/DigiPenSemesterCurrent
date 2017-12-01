/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file main.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 260
\date 2017/10/17
\brief
  Conatins the implementation for assignment 2.
*/
/*****************************************************************************/

#ifdef _WIN32
#include <Ws2tcpip.h>
#include <WinBase.h>
#else
#include <unistd.h>
#endif

#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>

//#include <vld.h>


#include "Socket.h"


#define DEBUG

#ifdef _WIN32
class WinSockLoader
{
public:
  static void Initialize();
  static void Purge();
private:
  static LPWSADATA _wsaData;
};

LPWSADATA WinSockLoader::_wsaData = nullptr;


void WinSockLoader::Initialize()
{
  _wsaData = new WSAData();
  int error = WSAStartup(MAKEWORD(2, 2), _wsaData);
#ifdef DEBUG
  if (error)
    throw("WSA Startup Error");
#endif // !DEBUG
}

void WinSockLoader::Purge()
{
  WSACleanup();
  delete _wsaData;
}
#endif // _WIN32

int main()
{ 
  bool running = true;
  std::ofstream outfile;
  outfile.open("output.txt");


  #ifdef _WIN32
  WinSockLoader::Initialize();
  #endif // !_WIN32
  
  // connect to my server
  SocketTCP socket;
  socket.Connect(8008, "127.0.0.1");
  std::cout << "Connected" << std::endl;
  std::string message("GET / HTTP/1.1\n"
                      "Host: echo.cs260.net\n\n");
  socket.Send(message.c_str(), message.size());

  while(true) {}
  #ifdef _WIN32
  WinSockLoader::Purge();
  #endif

  return 0;
}
