/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Socket.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 260
\date 2017/10/17
\brief
  Contains the implementation of the SocketTCP class.
*/
/*****************************************************************************/

#ifdef _WIN32
#include <Ws2tcpip.h>
#else
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#endif

#include "Socket.h"
#include <cstring>
#include <iostream>


#ifndef _WIN32
#define SOCKET_ERROR -1
#endif

SocketTCP::SocketTCP() :
  _block(true)
{
  // creating TCP Socket
  _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

SocketTCP::SocketTCP(const SOCKET & socket) :
  _socket(socket), _block(true)
{}

void SocketTCP::Connect(int port, const char * address)
{
  // getting address info using DNS
  addrinfo * address_info = nullptr;
  addrinfo * hints = new addrinfo();
  memset((void *)hints, 0, sizeof(addrinfo));
  getaddrinfo(address, "", hints, &address_info);
  sockaddr * sockaddr_address = address_info->ai_addr;
  // preparing address the socket will connect to
  sockaddr_in c_address = *reinterpret_cast<sockaddr_in *>(sockaddr_address);
  c_address.sin_family = AF_INET;
  c_address.sin_port = htons(port);
  int c_address_size = sizeof(sockaddr_in);
  // free the addrinfo
  delete hints;
  freeaddrinfo(address_info);
  // connect to address
  int result = connect(_socket, (sockaddr *)&c_address, c_address_size);
  if(result == SOCKET_ERROR)
    std::cout << "Connect Error: " << address << std::endl;
}

void SocketTCP::Bind(int port)
{
  // creating the address that messages should be sent to
  sockaddr_in back_address;
  // sin_addr is a union. S_addr fills that entire union
  #ifdef _WIN32
  back_address.sin_addr.S_un.S_addr = INADDR_ANY;
  #else
  back_address.sin_addr.s_addr = INADDR_ANY;
  #endif

  back_address.sin_family = AF_INET;
  back_address.sin_port = htons(port);
  // bindin socket
  bind(_socket, (sockaddr *)&back_address, sizeof(sockaddr));
}

int SocketTCP::Send(const char * data, int bytes)
{
  int result = send(_socket, data, bytes, 0);
  if (result == SOCKET_ERROR) {
    #ifdef _WIN32
    int error = WSAGetLastError();
    if (error == WSAEWOULDBLOCK)
      return SEND_SOCKET_FULL;
    #else
    if (errno == EWOULDBLOCK)
     return SEND_SOCKET_FULL;
    #endif
    // other error occured
  }
  return result;
}

int SocketTCP::Recieve(char * data, int bytes)
{
  // do error checking
  int result = recv(_socket, data, bytes, 0);
  if (result == SOCKET_ERROR) {
    #ifdef _WIN32
    int error = WSAGetLastError();
    if (error == WSAEWOULDBLOCK)
      return RECIEVE_NO_DATA;
    #else
    if (errno == EWOULDBLOCK)
     return RECIEVE_NO_DATA;
    #endif
    // other error occured
  }
  return result;
}

void SocketTCP::Listen(int backlog)
{
  int result;
  result = listen(_socket, backlog);
  ErrorCheck(result);
}

SocketTCP * SocketTCP::Accept()
{
  // get accepted socket
  sockaddr from_address;
  #ifdef _WIN32
  int from_address_size = sizeof(sockaddr_in);
  #else
  unsigned int from_address_size = sizeof(sockaddr_in);
  #endif
  SOCKET accepted_socket;
  accepted_socket = accept(_socket, &from_address, &from_address_size);
  // see if socket was accepted
  #ifdef _WIN32
  if(accepted_socket == INVALID_SOCKET)
  #else
  if(accepted_socket == -1)
  #endif
    return nullptr;
  return new SocketTCP(accepted_socket);

}

void SocketTCP::Block(bool block)
{
  if (_block == block)
    return;
  #ifdef _WIN32
  unsigned long to_block = block ? 0 : 1;
  int result = ioctlsocket(_socket, FIONBIO, &to_block);
  #else
  int flags = fcntl(_socket, F_GETFL, 0);
  flags = block ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
  int result = fcntl(_socket, F_SETFL, flags);
  #endif
  ErrorCheck(result);
  _block = block;
}

void SocketTCP::CloseConnection()
{
  #ifdef _WIN32
  int result = shutdown(_socket, SD_BOTH);
  #else
  int result = shutdown(_socket, SHUT_RDWR);
  #endif
  ErrorCheck(result);
}

void SocketTCP::CloseRecieve()
{
  #ifdef _WIN32
  int result = shutdown(_socket, SD_RECEIVE);
  #else
  int result = shutdown(_socket, SHUT_RD);
  #endif
  ErrorCheck(result);
}

void SocketTCP::CloseSend()
{
  #ifdef _WIN32
  int result = shutdown(_socket, SD_SEND);
  #else
  int result = shutdown(_socket, SHUT_WR);
  #endif
  ErrorCheck(result);
}

int SocketTCP::ErrorCheck(int return_value)
{
  if (return_value == SOCKET_ERROR) {
    std::cout << "Error Occurred" << std::endl;
  }
  return 0;
}
