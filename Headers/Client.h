#include<string>
using namespace std;
class Client
{
   SOCKET listenSocket;
   struct sockaddr_in  srv;
   public:
     Client();
     ~Client();
     void createClientSocket();
     int initializeWSA();
     int connectToServer(const char* ipAddress, int port);
     int sendName(string&);
     int sendData(const char*);
     SOCKET getSocketID();
};