/* All content(c) 2016 - 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file main.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 260 Assignment 1
\date 19/09/2017
\brief
  Sends a UDP packet to 104.131.138.5:8888, receives a UDP packet from the
  same server and prints the packet's content.
*/
/*****************************************************************************/
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include <iostream>

// Size of the buffer for receiving data
#define RECEIVE_BUFFFER_SIZE 500

// Define for error debugging
//#define DE

// Checks the return value from a WinSock function
// and prints the error value if an error occured
#define CHECKWSAERROR(result, description)\
if (result == -1) {\
  error = WSAGetLastError();\
  std::cout << ##description##" Error: " << error << std::endl;\
}\

void main(int argc, char * argv[])
{
  // initialize winsock
  LPWSADATA wsa_data = new WSAData();
  int error = WSAStartup(MAKEWORD(2,2), wsa_data);
#ifdef DE
  if (error) {
    std::cout << "WSAStartup Error: " << error << std::endl;
    return;
  }
#endif
  // creating socket to send from
  SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // address to send to
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(8888);
  InetPton(AF_INET, "104.131.138.5", &address.sin_addr);
  int address_size = sizeof(sockaddr_in);
  // sending buffer to server
  int buffer_len = static_cast<int>(strlen(argv[0]));
  sendto(udp_socket, argv[0], buffer_len, 0, 
    (sockaddr *)&address, address_size);
  // receiving different buffer from server
  char receive_buffer[RECEIVE_BUFFFER_SIZE];
  int received_buffer_len = recvfrom(udp_socket, receive_buffer, 
    RECEIVE_BUFFFER_SIZE, 0, (sockaddr *)&address, &address_size);
#ifdef DE
  CHECKWSAERROR(received_buffer_len, "recvfrom")
#endif
  // putting null terminator on receive buffer and printing
  receive_buffer[received_buffer_len] =  '\0';
  std::cout << receive_buffer << std::endl;
  // closing socket and clean up winsock
  shutdown(udp_socket, SD_BOTH);
  closesocket(udp_socket);
  WSACleanup();
}