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

bool FindIsChunked(const std::string & current_data)
{
  if(current_data.find("Transfer-Encoding: chunked") == std::string::npos)
    return false;
  return true;
}

int FindContentStart(const std::string & current_data)
{
  size_t start = current_data.find("\r\n\r\n");
  if(start == std::string::npos)
    return -1;
  start += 4;
  return (int)start;
}

inline void ParseAddress(const std::string & address, std::string * domain_name,
  std::string * file_path)
{
  // find start and end of domain name
  size_t domain_start = address.find("//");
  if (domain_start != std::string::npos)
    domain_start += 2;
  size_t domain_end;
  if (domain_start == std::string::npos)
    domain_end = address.find("/");
  else
    domain_end = address.find("/", domain_start);
  // parse into domain name and file path
  if (domain_end == std::string::npos) {
    if (domain_start == std::string::npos)
      *domain_name = address;
    else
      *domain_name = address.substr(domain_start);
    *file_path = "/";
  }
  else {
    if (domain_start == std::string::npos)
      *domain_name = address.substr(0, domain_end);
    else
      *domain_name = address.substr(domain_start, domain_end - domain_start);
    *file_path = address.substr(domain_end);
  }
}

int main(int argc, char * argv[])
{ 
  std::ofstream outfile;
  outfile.open("output.txt");


  #ifdef _WIN32
  WinSockLoader::Initialize();
  #endif // !_WIN32
  // make sure we have an address
  if (argc < 2) {
    std::cout << "Request address not included" << std::endl;
    return 0;
  }
  std::string address(argv[1]);
  std::string domain_name;
  std::string file_path;
  ParseAddress(address, &domain_name, &file_path);
  // create the socket for http connection
  SocketTCP socket(80, domain_name.c_str());
  socket.Connect();
  socket.Block(false);
  // construct http get request and send
  std::string get_request;
  get_request.append("GET ");
  get_request.append(file_path);
  get_request.append(" HTTP/1.1\n");
  get_request.append("Host: ");
  get_request.append(domain_name);
  get_request.append(":80\n\n");
  socket.Send(get_request.c_str(), get_request.size());
  // recieve data
  std::string data;
  int content_length = -1;
  int content_start = -1;
  bool chunked = false;
  bool data_received = false;
  while(!data_received)
  {
    // receive from the socket
    char recv_buff[MTU_SIZE];
    int recv_bytes = RECIEVE_NO_DATA;
    recv_bytes = socket.Recieve(recv_buff, MTU_SIZE);
    // if we received data
    if (recv_bytes > 0)
    {
      data.append(recv_buff, recv_bytes);
      //std::cout << data;
      if (content_length == -1 && !chunked) {
        content_length = FindContentLength(data);
        if (content_length == -1)
          chunked = FindIsChunked(data);
      }

      if(content_start == -1)
        content_start = FindContentStart(data);

      if (content_length > 0 && content_start > 0) {
        if(content_length == data.length() - content_start)
          data_received = true;
      }

      if (chunked) {
        int end_start = data.size() - 7;
        if(data.substr(end_start) == "\r\n0\r\n\r\n")
          data_received = true;
      }
    }
  }
  // shutdown socket
  socket.CloseSend();
  socket.CloseRecieve();
  #ifdef _WIN32
  WinSockLoader::Purge();
  #endif
  // print out html content
  outfile << data << std::endl;

  return 0;
}
