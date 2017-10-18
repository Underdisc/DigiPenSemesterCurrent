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


#include "Socket.h"


#define DEBUG


int main(int argc, char * argv[])
{
  #ifdef _WIN32
  LPWSADATA wsa_data = new WSAData();
  int error = WSAStartup(MAKEWORD(2, 2), wsa_data);
  #ifdef DEBUG
  if (error) {
    std::cout << "WSAStartup Error: " << error << std::endl;
    return 0;
  }
  #endif // !DEBUG
  #endif // !_WIN32


  SocketTCP socket(8888, (char *)"104.131.138.5");
  socket.Connect();
  socket.Block(false);

  int bytes;
  if(argc > 0)
    bytes = (int)(strlen(argv[0]));
  else
    bytes = 0;
  socket.Send(argv[0], bytes);
  socket.CloseSend();
  std::cout << "Message Sent" << std::endl;

  std::string data;
  char recv_buff[MTU_SIZE];
  int recv_bytes = RECIEVE_NO_DATA;
  while(recv_bytes != RECIEVE_FIN){
    recv_bytes = socket.Recieve(recv_buff, MTU_SIZE);
    if (recv_bytes > 0)
      data.append(recv_buff, recv_bytes);
    if (recv_bytes == RECIEVE_NO_DATA){
      #ifdef _WIN32
      Sleep(100);
      #else
      usleep(100000);
      #endif
      std::cout << " .";
    }
  }
  std::cout << std::endl << data << std::endl;

  socket.CloseRecieve();

  #ifdef _WIN32
  WSACleanup();
  delete wsa_data;
  #endif
  return 0;
}
