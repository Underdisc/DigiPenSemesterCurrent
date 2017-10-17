
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include <string>
#include <iostream>
#include <errno.h>

#define MTU_SIZE 1500

#define DEBUG

class SocketTCP
{
public:
  SocketTCP(int port, char * ipv4_address) : _addressSize(sizeof(sockaddr_in)),
  _block(true)
  {
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _address.sin_family = AF_INET;
    _address.sin_port = htons(port); // host to network shorts
    InetPton(AF_INET, ipv4_address, &_address.sin_addr);
  }

  void Connect()
  {
    int result = connect(_socket, (sockaddr *)&_address, _addressSize);
    if(result == SOCKET_ERROR) {
      int error = WSAGetLastError();
      // THROW AN ERROR
    }
  }

  int Send(char * data, int bytes)
  {
    // make sure everything gets sent
    int result = send(_socket, data, bytes, 0);
    if(result = SOCKET_ERROR) {
      if(errno == EWOULDBLOCK)
        return -1;
      
      // check for non blocking
    }
    return result;
  }

  int Recieve(char * data, int bytes)
  {
    // do error checking
    int result = recv(_socket, data, bytes, 0);
    switch (result)
    {
    case SOCKET_ERROR:
      if (errno == EWOULDBLOCK)
        return -1;
      //throw error
    case 0:
      return 0;
    }
    return result;
  }

  void Block(bool block)
  {
    if(_block == block)
      return;
    unsigned long a = block ? 1 : 0;
    int result = ioctlsocket(_socket, FIONBIO, &a);
    if (result = SOCKET_ERROR) {
      // throw error
    }
    _block = block;
  }

  void Close()
  {
    // do any necessary error checking
    shutdown(_socket, SD_SEND);
    closesocket(_socket);
    int error = WSAGetLastError();
    if(error){
      std::cout << "fuk" << std::endl;
    }
  }




private:
  SOCKET _socket;
  sockaddr_in _address;
  int _addressSize;
  bool _block;
};

class SocketTCPError
{

};


int main(int argc, char * argv[])
{
  LPWSADATA wsa_data = new WSAData();
#ifdef DEBUG
  int error = WSAStartup(MAKEWORD(2,2), wsa_data);
  if (error) {
    std::cout << "WSAStartup Error: " << error << std::endl;
    return 0;
  }
#else
  WSAStartup(MAKEWORD(2,2), wsa_data);
#endif // !DEBUG


  SocketTCP socket(8888, "104.131.138.5");
  socket.Connect();
  socket.Block(false);

  int bytes = (int)(strlen(argv[0]));
  socket.Send(argv[0], bytes);

  std::string data;
  char recv_buff[MTU_SIZE];
  int recv_bytes = -1;
  while(recv_bytes != 0){
    recv_bytes = socket.Recieve(recv_buff, MTU_SIZE);
    if (recv_bytes > 0)
      data.append(recv_buff, recv_bytes);
  }

  std::cout << data << std::endl;

  socket.Close();

  WSACleanup();



  delete wsa_data;
  return 0;
}