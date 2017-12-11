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

#define CONNECT_DONE 0
#define CONNECT_IN_PROGRESS 1
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
  SocketTCP();
  SocketTCP(const SOCKET & socket);
  int SetConnectAddress(int port, const char * address);
  int Connect();
  int Bind(int port);
  int Send(const char * data, int bytes);
  int Recieve(char * data, int bytes);
  void Listen(int backlog);
  SocketTCP * Accept();
  void Block(bool block);
  void CloseConnection();
  void CloseRecieve();
  void CloseSend();
private:
  SOCKET _socket;
  sockaddr_in _connectAddress;
  bool _block;

  int ErrorCheck(int return_value);
};

#endif // !SOCKETTCP_H
