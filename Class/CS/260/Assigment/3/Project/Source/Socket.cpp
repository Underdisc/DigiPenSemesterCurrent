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


#ifndef _WIN32
#define SOCKET_ERROR -1
#endif

SocketTCP::SocketTCP(int port, const char * address) :
_addressSize(sizeof(sockaddr_in)), _block(true)
{
  _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  // getting address info
  addrinfo * address_info = nullptr;
  addrinfo * hints = new addrinfo();
  memset((void *)hints, 0, sizeof(addrinfo));
  getaddrinfo(address, "", hints, &address_info);
  sockaddr * sockaddr_address = address_info->ai_addr;
  // preparing socket address
  _address = *reinterpret_cast<sockaddr_in *>(sockaddr_address);
  _address.sin_family = AF_INET;
  _address.sin_port = htons(port);// host to network short
  // free the addrinfo
  freeaddrinfo(address_info);
}

void SocketTCP::Connect()
{
  int result = connect(_socket, (sockaddr *)&_address, _addressSize);
  ErrorCheck(result);
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
    // error occured
    // check WSAGetLastError
  }
  return 0;
}
