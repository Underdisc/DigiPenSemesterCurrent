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
#include <regex>

#include "Socket.h"

#define DEBUG

//--------------------// WinSockLoader //--------------------// 
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

//--------------------// Data Parsing Helpers //--------------------//
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
class HTTPProxyServer;

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
    WEB_TO_CLIENT,
  };
private:
  SocketTCP * _clientSocket;
  SocketTCP _webSocket;
  Stage _stage;

  std::string _data;
  // data parsing values
  int _contentLength;
  int _contentStart;
  int _chunked;

private:
  void ClientRecieve();
  bool WebSend();
  bool WebToClient();

  friend HTTPProxyServer;
};

HTTPProxy::HTTPProxy(SocketTCP * client_socket) :
  _clientSocket(client_socket), _webSocket(), _stage(CLIENT_RECIEVE), _data(),
  _contentLength(-1), _contentStart(-1), _chunked(false)
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
  case WEB_TO_CLIENT:
    done = WebToClient();
    break;
  }
  return done;
}

void HTTPProxy::Close()
{
  _clientSocket->CloseSend();
  _webSocket.CloseConnection();
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
  if (domain_start == (int)std::string::npos) {
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
  _stage = WEB_TO_CLIENT;
  _data.clear();
  return false;
}

bool HTTPProxy::WebToClient()
{
  char recv_buff[MTU_SIZE];
  int recv_bytes = RECIEVE_NO_DATA;
  recv_bytes = _webSocket.Recieve(recv_buff, MTU_SIZE);
  // if we received data
  if (recv_bytes > 0)
  {
    _data.append(recv_buff, recv_bytes);
    _clientSocket->Send(recv_buff, recv_bytes);
    if (_contentLength == -1 && !_chunked) {
      _contentLength = FindContentLength(_data);
      if (_contentLength == -1)
        _chunked = FindIsChunked(_data);
    }

    if (_contentStart == -1)
      _contentStart = FindContentStart(_data);

    if (_contentLength > 0 && _contentStart > 0) {
      if (_contentLength == (int)_data.length() - _contentStart){
        return true;
      }
    }

    if (_chunked) {
      int end_start = _data.size() - 7;
      if (_data.substr(end_start) == "\r\n0\r\n\r\n")
        return true;
    }
  }
  return false;
}

//------------------// HTTPProxyServer //------------------//
class HTTPProxyServer
{
public:
  HTTPProxyServer(int port);
  void Update();
  void Close();
  int _clientsServed;
private:
  SocketTCP _serverSocket;
  std::list<HTTPProxy> _clientProxies;
};


HTTPProxyServer::HTTPProxyServer(int port) :
  _clientsServed(0), _serverSocket(), _clientProxies()
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
  if (new_client){
    _clientProxies.push_back(HTTPProxy(new_client));
    std::cout << "Proxy Client Connected" << std::endl;
  }

  std::list<HTTPProxy>::iterator it = _clientProxies.begin();
  std::list<HTTPProxy>::iterator it_e = _clientProxies.end();
  while(it != it_e){
    bool done = it->Update();
    if(done){
      it->Close();
      delete it->_clientSocket;
      it = _clientProxies.erase(it);
      std::cout << "Proxy Client Served" << std::endl;
      ++_clientsServed;
    }
    else{
      ++it;
    }
  }
}

void HTTPProxyServer::Close()
{
  for (HTTPProxy & proxy : _clientProxies) {
    proxy.Close();
    delete proxy._clientSocket;
  }
}

//------------------// Main //------------------//
int main(int argc, char * argv[])
{
  bool running = true;

  #ifdef _WIN32
  WinSockLoader::Initialize();
  #endif // !_WIN32
  // make sure we have a port
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
  proxy_server.Close();

  #ifdef _WIN32
  WinSockLoader::Purge();
  #endif

  return 0;
}
