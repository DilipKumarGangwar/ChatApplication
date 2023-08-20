//Server Program
//Handles Multiple Clients

#include<iostream>
#include<vector>
#include<winsock2.h>

using namespace std;

#define PORT 9909
struct sockaddr_in  srv;

int nMaxFd =0;
const int MAX_CLIENTS = 5; // Maximum number of clients

int main()
{
    int nRet = 0;
    //Initialise the WSA Variables
    WSADATA ws;
    if(WSAStartup(MAKEWORD(2,2),&ws)<0)
    {
        cout<<"WSA failed to initialise";
        return 1;
    }

    else
       cout<<"WSA initialised"; 

    //initialise the Socket
    SOCKET listenSocket = INVALID_SOCKET;
    listenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listenSocket == INVALID_SOCKET)
    {
       cout<<"Socket Creation FAILED: Error at Socket "<<WSAGetLastError();
       WSACleanup(); // WSACleanup is used to terminate the use of the WS2_32 DLL.
       return 1;
    }
    else
       cout<<"Socket Created SUCCESSFULLY with Socket ID: "<<listenSocket<<endl; 

    //Initialise the environment for sockaddr structure
    srv.sin_family=AF_INET;
    srv.sin_port = htons(PORT);
    //srv.sin_addr.s_addr = inet_addr("127.0.0.1");  
    srv.sin_addr.s_addr = INADDR_ANY;  
   // memset(&(srv.sin_zero),0,8);

    memset(&(srv.sin_zero),0,sizeof(srv.sin_zero));
   
    //BIND Socket to the local port  or listener
    // Setup the TCP listening socket

    //The bind function (winsock.h) associates a local address with a socket.
    nRet = bind( listenSocket, (sockaddr*)&srv,sizeof(sockaddr));
    if(nRet == SOCKET_ERROR)
    {
      cout<<"FAILED to bind Socket to the local port "<<endl;
      wprintf(L"bind failed with error %u\n", WSAGetLastError());
      closesocket(listenSocket);
      WSACleanup();
      return 1;
    }

    else
      cout<<"BIND is SUCCESSFUL"<<endl;  

    //Listen to the requests coming from client
    nRet = listen(listenSocket,5);
    if(nRet<0)
      cout<<"FAILED to start listen listen on the local port "<<endl;
    else
      cout<<"SUCCESS: Started to listen on the local port"<<endl; 
    

    //Create vector to store Client Sockets

    vector<int> clientSockets(MAX_CLIENTS,0);

    fd_set read_fd_set;
    
    while(1)
    {
        FD_ZERO(&read_fd_set);
        FD_SET(listenSocket,&read_fd_set);  //if listenSocket is not set in read_fd_set, then insert it   
         
         // Add client sockets to the set, (It will unnecessarily copy previous data also)
         for(int i=0;i<MAX_CLIENTS;i++)
         {
            if(clientSockets[i] != 0)
            {
              FD_SET(clientSockets[i],&read_fd_set);
            }
         }


        // Use select to monitor multiple sockets 
        cout<<"Monitoring Sockets for Events.."<<endl;  
        int ret_val = select(0,&read_fd_set,NULL,NULL,NULL);

        if (ret_val < 0) {
            cout << "select() failed" << endl;
            break;
        }
        cout<<ret_val;

        // Check if the listen socket has an event like some new client wants to connect
        if(FD_ISSET(listenSocket,&read_fd_set))
        {
          //accept the request for this new client
          //Create new socket and add it to client vector
          SOCKET newClientSocket = accept(listenSocket,NULL,NULL);
          if(newClientSocket !=INVALID_SOCKET)
          {
               // Add the new client socket to the client array
               cout<<"New Client Added="<<newClientSocket<<endl;
               
               //add in the first empty space
               for(int i=0;i<MAX_CLIENTS;i++)
               {
                 if(clientSockets[i] == 0)
                 {
                    clientSockets[i] = newClientSocket;
                    break;
                 }
               }
          }
        }

        cout<<"Client Array After Adding New Client:"<<endl;
        for (int i = 0; i < MAX_CLIENTS; i++) 
              cout<<clientSockets[i]<<endl; 

         // Check client sockets for events (e.g. some data has come for reading)
        for (int i = 0; i < MAX_CLIENTS; i++) 
        {
          cout<<"Checking for incoming data..."<<endl;
          if(clientSockets[i]!=0 && FD_ISSET(clientSockets[i],&read_fd_set))
          {
            //if Yes
              char buffer[1024]={0};

              //receive data
              int nStatus = recv(clientSockets[i],buffer,1024,0);
              if(nStatus < 0)
              {
                cout<<"Client "<<clientSockets[i]<<" Disconnected from Server "<<endl;
                //close the socket
                closesocket(clientSockets[i]);
                clientSockets[i] = 0;
              }  
              else if(nStatus >0)
                  cout<<"Message Received from Client= "<<buffer<<endl;
          }

        }

    }

    // Close client sockets
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clientSockets[i] != 0) 
            closesocket(clientSockets[i]);
    }

   //close listener socket 
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
