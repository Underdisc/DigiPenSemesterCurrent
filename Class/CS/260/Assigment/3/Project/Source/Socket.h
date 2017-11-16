/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Socket.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 260
\date 2017/10/17
\brief
  Contains the interface for the SocketTCP class. Used for creating and
  managing a TCP socket.
*/
/*****************************************************************************/

#ifndef SOCKETTCP_H
#define SOCKETTCP_H

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifndef _WIN32
typedef int SOCKET;
#endif

#define MTU_SIZE 1500

#define SEND_SOCKET_FULL -1
#define RECIEVE_NO_DATA -1
#define RECIEVE_FIN 0

/*****************************************************************************/
/*!
\class SocketTCP
\brief
  Used for TCP socket creation and management.
*/
/*****************************************************************************/
class SocketTCP
{
public:
  SocketTCP(int port, const char * address);
  void Connect();
  int Send(const char * data, int bytes);
  int Recieve(char * data, int bytes);
  void Block(bool block);
  void CloseRecieve();
  void CloseSend();
private:
  SOCKET _socket;
  sockaddr_in _address;
  int _addressSize;
  bool _block;

  int ErrorCheck(int return_value);
};

#endif // !SOCKETTCP_H
