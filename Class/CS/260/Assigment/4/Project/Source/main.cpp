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
#include <list>
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

//------------------// HTTPProxy //------------------//
class HTTPProxy
{
public:
  HTTPProxy(SocketTCP * client_socket);
  bool Update();
  void Close();
private:
  enum Stage
  {
    CLIENT_RECIEVE,
    WEB_SEND,
    WEB_RECIEVE,
    CLIENT_SEND,
  };
private:
  SocketTCP * _clientSocket;
  SocketTCP _webSocket;
  Stage _stage;
  std::string _data;
private:
  void ClientRecieve();
  bool WebSend();
  void WebReceive();
  bool ClientSend();
};

HTTPProxy::HTTPProxy(SocketTCP * client_socket) :
  _clientSocket(client_socket), _webSocket(), _stage(CLIENT_RECIEVE), _data()
{
  _clientSocket->Block(false);
}

bool HTTPProxy::Update()
{
  bool done = false;
  switch (_stage)
  {
  case CLIENT_RECIEVE:
    ClientRecieve();
    break;
  case WEB_SEND:
    done = WebSend();
    break;
  case WEB_RECIEVE:
    WebReceive();
    break;
  case CLIENT_SEND:
    done = ClientSend();
    break;
  }
  return done;
}

void HTTPProxy::Close()
{

}

void HTTPProxy::ClientRecieve()
{
  char recv_buff[MTU_SIZE];
  int recv_len = _clientSocket->Recieve(recv_buff, MTU_SIZE);
  _data.append(recv_buff, recv_len);
  if (_data.size() >= 2) {
    std::string end(_data.substr(_data.size() - 2));
    if(end == "\n\n"){
      _stage = WEB_SEND;
    }
  }
}

bool HTTPProxy::WebSend()
{
  // extract the domain name
  int domain_start = _data.find("Host: ");
  if (domain_start == std::string::npos) {
    // not a valid GET request
    return true;
  }
  domain_start += 6;
  int domain_end = _data.find("\n", domain_start);
  std::string domain_name(_data.substr(domain_start, 
    domain_end - domain_start));
  // connect the web socket and send request
  _webSocket.Connect(80, domain_name.c_str());
  _webSocket.Send(_data.c_str(), _data.size());
  _webSocket.Block(false);
  _stage = WEB_RECIEVE;
  return false;
}

void HTTPProxy::WebReceive()
{
  std::cout << "Recieving Web data" << std::endl;
}

bool HTTPProxy::ClientSend()
{
  return false;
}


//------------------// HTTPProxyServer //------------------//
class HTTPProxyServer
{
public:
  HTTPProxyServer(int port);
  void Update();
private:
  SocketTCP _serverSocket;
  std::list<HTTPProxy> _clientProxies;
};


HTTPProxyServer::HTTPProxyServer(int port) : 
  _serverSocket(), _clientProxies()
{
  _serverSocket.Bind(port);
  _serverSocket.Listen(10);
  _serverSocket.Block(false);
  std::cout << "- Server Live -" << std::endl;
}

void HTTPProxyServer::Update()
{
  SocketTCP * new_client;
  new_client = _serverSocket.Accept();
  if (new_client) {
    _clientProxies.push_back(HTTPProxy(new_client));
    std::cout << "Proxy Client Connected" << std::endl;
  }

  std::list<HTTPProxy>::iterator it = _clientProxies.begin();
  std::list<HTTPProxy>::iterator it_e = _clientProxies.end();
  while(it != it_e){
    bool done = it->Update();
    if(done){
      it->Close();
      it = _clientProxies.erase(it);
    }
    else{
      ++it;
    }
  }
}

//------------------// Main //------------------//
int main(int argc, char * argv[])
{ 
  bool running = true;
  std::ofstream outfile;
  outfile.open("output.txt");


  #ifdef _WIN32
  WinSockLoader::Initialize();
  #endif // !_WIN32
  // make sure we have an address
  if (argc < 2) {
    std::cout << "Port number not included" << std::endl;
    return 0;
  }
  int port = atoi(argv[1]);
  // create the socket for http connection
  HTTPProxyServer proxy_server(port);
  while (running) {
    proxy_server.Update();
  }
  
  /*SocketTCP socket;
  socket.Bind(port);
  socket.Listen(20);
  socket.Block(false);
  std::vector<SocketTCP *> accepted_sockets;
  while (running) {
    SocketTCP * new_accpeted_socket;
    new_accpeted_socket = socket.Accept();
    if(new_accpeted_socket){
      accepted_sockets.push_back(new_accpeted_socket);
      std::cout << "Made Connection" << std::endl;
    }

    for (SocketTCP * client_socket : accepted_sockets) {
      client_socke
    }


  }*/
  
  
  
  // this will be needed when you recieve request data as the proxy
  // recieve data
  /*std::string data;
  int content_length = -1;
  int content_start = -1;
  bool chunked = false;
  bool data_received = false;*/


  // should I constantly be looking for the chunks as I recieve data, or should
  // I wait to see "\r\n0\r\n\r\n".

  // Then Is it better to parse that data once we recieve it or while we are
  // recieving it.

  /*while(!data_received)
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
  }*/
  // shutdown socket
  
  
  //socket.CloseSend();
  //socket.CloseRecieve();
  #ifdef _WIN32
  WinSockLoader::Purge();
  #endif
  // print out html content
  //outfile << data << std::endl;

  return 0;
}
