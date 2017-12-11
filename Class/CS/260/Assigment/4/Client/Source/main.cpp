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
  SocketTCP socket1;
  socket1.Connect(8008, "127.0.0.1");
  socket1.Block(false);
  std::cout << "Connected" << std::endl;
  std::string message1("GET / HTTP/1.1\r\nHost: echo.cs260.net\r\n\r\n");
  socket1.Send(message1.c_str(), message1.size());


  SocketTCP socket2;
  socket2.Connect(8008, "127.0.0.1");
  socket2.Block(false);
  std::cout << "Connected" << std::endl;
  std::string message2("GET / HTTP/1.1\r\nHost: echo.cs260.net\r\n\r\n");
  socket2.Send(message2.c_str(), message2.size());

  std::string data1;
  std::string data2;
  int doh = 0;
  while(doh < 2)
  {
    char recv_buff1[MTU_SIZE];
    int recv_len1 = socket1.Recieve(recv_buff1, MTU_SIZE);
    char recv_buff2[MTU_SIZE];
    int recv_len2 = socket2.Recieve(recv_buff2, MTU_SIZE);
    if (recv_len1 > 0) {
      data1.append(recv_buff1, recv_len1);
      ++doh;
    }

    if (recv_len2 > 0) {
      data2.append(recv_buff2, recv_len2);
      ++doh;
    }

  }
  std::cout << data1 << std::endl;
  std::cout << "=================================================" << std::endl;
  std::cout << data2 << std::endl;
  #ifdef _WIN32
  WinSockLoader::Purge();
  #endif

  return 0;
}
