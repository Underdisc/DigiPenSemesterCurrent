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


int FindContentLength(const std::string & current_data)
{
  std::smatch matches;
  std::regex content_length_expression("Content-Length: ([0-9]+)",
    std::regex_constants::icase);
  std::regex_search(current_data, matches, content_length_expression);
  if(matches.size() > 0){
    const std::string & capture = matches[1];
    return atoi(capture.c_str());
  }
  return -1;
}

int FindContentStart(const std::string & current_data)
{
  size_t start = current_data.find("\r\n\r\n");
  if(start == std::string::npos)
    return -1;
  start += 4;
  return (int)start;
}

int main(int argc, char * argv[])
{ 
  #ifdef _WIN32
  WinSockLoader::Initialize();
  #endif // !_WIN32
  // make sure we have an address
  if (argc < 2) {
    std::cout << "Request address not included" << std::endl;
    return 0;
  }
  std::string address(argv[1]);
  // get the domain name and file
  size_t domain_start = address.find("//") + 2;
  size_t domain_end = address.find("/", domain_start);
  std::string domain_name(address, domain_start, domain_end - domain_start);
  std::string file_path(address, domain_end);
  // create the socket for http connection
  SocketTCP socket(80, domain_name.c_str());
  socket.Connect();
  socket.Block(false);
  // construct http get request and send
  std::string get_request;
  get_request.append("GET ");
  get_request.append(file_path);
  get_request.append(" HTTP/1.1\n"); // / is the path to the file
  get_request.append("Host: ");
  get_request.append(domain_name); // This should be the domain name
  get_request.append(":80\n\n");
  socket.Send(get_request.c_str(), get_request.size());
  // recieve data
  std::string data;
  int content_length = -1;
  int content_start = -1;
  while(content_start == -1 && 
    content_length != (int)data.size() - content_start)
  {
    // receive from the socket
    char recv_buff[MTU_SIZE];
    int recv_bytes = RECIEVE_NO_DATA;
    recv_bytes = socket.Recieve(recv_buff, MTU_SIZE);
    // if we received data
    if (recv_bytes > 0)
    {
      data.append(recv_buff, recv_bytes);
      // seeing if we have found the content length
      if (content_length == -1)
        content_length = FindContentLength(data);
      // seeing if we have recieved all the data
      if (content_length > 0 && content_start == -1)
        content_start = FindContentStart(data);
    }
  }
  // shutdown socket
  socket.CloseSend();
  socket.CloseRecieve();
  #ifdef _WIN32
  WinSockLoader::Purge();
  #endif
  // print out html content
  std::string content(data.substr(content_start, content_length));
  std::cout << content << std::endl;
  return 0;
}
