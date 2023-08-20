#include<WinSock2.h>
#include<vector>
#include<algorithm>
using namespace std;


class Server
{
   SOCKET listenSocket;
   struct sockaddr_in  srv;
   public:
     Server();
     ~Server();
     void createServerSocket();
     int initializeWSA();
     int bindToSocket(int port,const char* ipAddress="");
     int startListen(int);
     void acceptAndProcessTheRequests(vector<pair<SOCKET,string>> &,fd_set& );
     void addClientsToFDSet(vector<pair<SOCKET,string>> &,fd_set& );
     void checkConnectionRequestOnMainSocket(vector<pair<SOCKET,string>> &,fd_set& );
     void checkEventOnClientSocket(vector<pair<SOCKET,string>> &,fd_set& );
     SOCKET getSocketID();
};