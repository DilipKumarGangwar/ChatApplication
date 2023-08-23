//Server Program
//Handles Multiple Clients

#include<iostream>
#include "..\Headers\Server.h"
#include<vector>
#include<thread>
#include<string>
#include<winsock2.h>
#include<map>
using namespace std;

#define PORT 9909
#define DATA_BUFFER 1024
struct sockaddr_in  srv;

int nMaxFd =0;
const int MAX_CLIENTS = 5; // Maximum number of clients to handle
fd_set master;

map<SOCKET, std::string> clientInfo;


Server::Server()
{
   listenSocket = INVALID_SOCKET;
}

Server::~Server()
{
  if (listenSocket != INVALID_SOCKET) 
            closesocket(listenSocket);
   
  WSACleanup();
}

int Server::initializeWSA()
{
    WSADATA ws;
    if(WSAStartup(MAKEWORD(2,2),&ws)<0)
        exit(EXIT_FAILURE);
    else      
       return EXIT_SUCCESS;    
}

void Server::createServerSocket()
{
    listenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
}

SOCKET Server::getSocketID()
{
    return listenSocket;
}

int Server::bindToSocket(int port,const char *ip )
{
   //fill server address
    srv.sin_family=AF_INET;
    srv.sin_port = htons(port); 
    if(!strcmp(ip,""))  //or ip[0]=='\0'
    {
      srv.sin_addr.s_addr = INADDR_ANY;
    }
    else  
    {
      srv.sin_addr.s_addr =  inet_addr(ip);
    }
    //srv.sin_addr.s_addr = INADDR_ANY;
    memset(&(srv.sin_zero),0,sizeof(srv.sin_zero));

   //binding to local port and ip
   return bind(listenSocket,(struct sockaddr*)&srv,sizeof(srv));
   
}

int Server::startListen(int no_Of_Requests)
{
    return listen(listenSocket,no_Of_Requests); //start listeneing to client rquests
}

void Server :: addClientsToFDSet(vector<pair<SOCKET,string>> &clientInfo,fd_set &read_fd_set)
{
  // Add client sockets to the set, (It will unnecessarily copy previous data also)
    for(int i=0;i<MAX_CLIENTS;i++)
    {
        if(clientInfo[i].first != 0)
        {
          FD_SET(clientInfo[i].first,&read_fd_set);
        }
    }
}

// void Server::checkConnectionRequestOnMainSocket(vector<pair<SOCKET,string>> &clientInfo,fd_set &read_fd_set)
// {
//           if(FD_ISSET(listenSocket,&read_fd_set))
//           {
//             //accept the request for this new client
//             //Create new socket and add it to client vector
//             SOCKET newClientSocket = accept(listenSocket,NULL,NULL);
//             if(newClientSocket !=INVALID_SOCKET)
//             {
//                 // Add the new client socket to the client array
//                 cout<<"New Client Added="<<newClientSocket<<endl;
                
//                 //add in the first empty space
//                 for(int i=0;i<MAX_CLIENTS;i++)
//                 {
//                   if(clientInfo[i].first == 0)
//                   {
//                       clientInfo[i].first = newClientSocket;
//                       clientInfo[i].second = ""; // Initialize the name as empty
//                       break;
//                   }
//                 }
//             }
//           }
// }

// void Server::checkEventOnClientSocket(vector<pair<SOCKET,string>> &clientInfo,fd_set &read_fd_set)
// {
//       for (int i = 0; i < MAX_CLIENTS; i++) 
//       {
//         //cout<<"Checking for incoming data..."<<endl;
//         if(clientInfo[i].first!=0 && FD_ISSET(clientInfo[i].first,&read_fd_set))
//         {
//           //if Yes
//             char buffer[DATA_BUFFER]={0};

//             //receive data
//             int nStatus = recv(clientInfo[i].first,buffer,DATA_BUFFER,0);
//            //  cout<<"Recreived="<<buffer<<" "<<nStatus<<endl;
//             if(nStatus <= 0)
//             {
//               cout<<"Client "<<clientInfo[i].first<<" with name= "<<clientInfo[i].second<<" Disconnected from Server "<<endl;
//               //close the socket
//               closesocket(clientInfo[i].first);
//               clientInfo[i].first = 0;
//               clientInfo[i].second = "";  //make name as empty
//             }  
//             else if(nStatus >0)
//             {
//                 if(clientInfo[i].second.empty())
//                 {
//                   clientInfo[i].second = buffer; //add the name of client
//                   cout<<"Client "<<clientInfo[i].first<<" with name= "<<clientInfo[i].second<<" CONNECTED "<<endl;
//                 }
//                 else
//                 {
//                     cout<<"Message Received from Client : "<<clientInfo[i].second<<"("<<clientInfo[i].first<<") = "<<buffer<<endl;
//                     // forward the same message back to each of the clients
//                     for(int j=0;j<MAX_CLIENTS;j++)
//                     {
//                       if(i!=j)
//                       {
//                         string message =  buffer;
//                         int sendStatus = send(clientInfo[j].first, message.c_str(), message.size()+1, 0);
//                         memset(buffer, 0, DATA_BUFFER); // Clear the buffer
//                         //cout<<"send"<<sendStatus<<endl;
//                         //if(sendStatus == SOCKET_ERROR)
//                           //  cout<<"Failed to send Message to client "<<clientInfo[j].second<<"("<<clientInfo[j].first<<") = "<<endl;
//                       }
//                     }
//                 }
//             }
//         }

//       }
// }



// void Server::acceptAndProcessTheRequests(vector<pair<SOCKET,string>> &clientInfo,fd_set &read_fd_set)
// {
      
//       SOCKET listenSocket =  this->getSocketID();  //=>server.getSocketID();
//       while(1)
//       {
//           FD_ZERO(&read_fd_set);
//           FD_SET(listenSocket,&read_fd_set);  //if listenSocket is not set in read_fd_set, then insert it   
          
//           this->addClientsToFDSet(clientInfo,read_fd_set);

//          struct timeval timeout;
//         timeout.tv_sec = 3;  // Set timeout to 5 seconds
//         timeout.tv_usec = 0;
//           // Use select to monitor multiple sockets 
//           cout<<"Monitoring Sockets for Events.."<<endl;  
//           int ret_val = select(0,&read_fd_set,NULL,NULL,0);
          

//           if (ret_val < 0) {
//               cout << "select() failed" << endl;
//               break;
//           }
//           //cout<<ret_val;

//           // Check if the listen socket has an event like some new client wants to connect
//           this->checkConnectionRequestOnMainSocket(clientInfo,read_fd_set); 

//           cout<<"Client Array After Adding New Client:"<<endl;
//           for (int i = 0; i < MAX_CLIENTS; i++) 
//                 cout<<clientInfo[i].first<<" "<<clientInfo[i].second<<endl; 
          
//           // Check client sockets for events (e.g. some data has come for reading)
//           this->checkEventOnClientSocket(clientInfo,read_fd_set);

//       } //end of while

// }

void handleNewConnection(SOCKET socket)
{
   char clientName[DATA_BUFFER];
   //receive client name
    recv(socket,clientName,DATA_BUFFER,0);
    //1.handle welcome message
    //Preapare welcome message 
    //Send it to all connected clients to inform them of arrival of new client/member
   
   string clientNamestr = clientName;

  clientInfo[socket] = clientNamestr;
   

    //Prepare welcome message
    string welcome_message = "Welcome to Chat Group";
    welcome_message = welcome_message + clientName; 
    //send welcome message to all clients
    for(int i=0;i<master.fd_count;i++)
       send(master.fd_array[i],welcome_message.c_str(),strlen(welcome_message.c_str()),0);

    //2. listen the receive requests  on this  socket
    while(1)
    {
      char recvbuffer[DATA_BUFFER]={0};
      int recvBytes= recv(socket,recvbuffer,sizeof(recvbuffer),0);
      //success
      if(recvBytes>0)
      {
         cout<<'\r';
         cout<<recvbuffer<<endl;
         cout<<"Server:";
         
         for(int i=0;i<master.fd_count;i++)
         {
          if(master.fd_array[i] != socket)
             send(master.fd_array[i],recvbuffer,strlen(recvbuffer),0);
         }
      }
      else if(recvBytes == 0)
      {
          FD_CLR(socket, &master);
          std::cout << '\r';
          std::cout << "A client disconnected" << endl;
          std::cout << "Server: ";
          //tell others
          
          for (int i = 0; i < master.fd_count; i++) 
          {
            char message[] = "Server : A client disconnected";
             send(master.fd_array[i], message, strlen(message), 0);  
          } 
          return;

      }
      else
      {
          FD_CLR(socket, &master);
          std::cout << '\r';
          std::cout << "Receive failed from a client: " << WSAGetLastError() << endl;
          cout<<clientInfo[socket]<<" has left"<<endl;
          std::cout << "Server: ";
          
          //tell others          
          string message = "Server : " + clientInfo[socket] + " has left";
      
          for (int i = 0; i < master.fd_count; i++) 
             send(master.fd_array[i], message.c_str(), message.length()+1, 0);  
          
          return ;
          
      }
    }


}

void handleAcceptRequest(SOCKET socket)
{
   //keep on listeneing 
   while(1)
   {
      //accept the request for this new client
      //Create new socket
      SOCKET newClientSocket = accept(socket,nullptr,nullptr);
      if(newClientSocket != INVALID_SOCKET)
      {
          //accept is successfull
          //create new thread which keep on listeening on this new socket
          FD_SET(newClientSocket,&master);
          thread newConnection(handleNewConnection,newClientSocket);          
          newConnection.detach();
      }
      else
      {
         cout<<'\r';
         cout<<"Accept request failed.."<<WSAGetLastError()<<endl;
         cout<<"Server:"<<endl;
         closesocket(newClientSocket);
      }
   }
}

void handleSendRequest(SOCKET socket)
{
    string data;
    do
    {
        std::cout << "Server: ";
        getline(std::cin, data);
        data = "Server: " + data;
        for (auto i = 0; i < master.fd_count; i++)
        {
            auto result = send(master.fd_array[i], data.c_str(), strlen(data.c_str()), 0);
            if (result == SOCKET_ERROR) {
                std::cout << '\r';
                std::cout << "Send to client " << i << " failed: " << WSAGetLastError() << endl;
                std::cout << "Server: ";
                return;
            }
        }
    } while (data.size() > 0);
}

int main()
{

      Server server;

      //Create Listener Socket
      server.createServerSocket();
      
      int nRet = 0;
    
      //Step 1: Initialise the WSA Variables
      if(server.initializeWSA())  
          exit(EXIT_FAILURE);  

      //Step 2:create a TCP Socket
      server.createServerSocket();
      if(server.getSocketID() == INVALID_SOCKET)
      {
        cout<<"Socket Creation FAILED: Error at Socket "<<WSAGetLastError();
        WSACleanup(); // WSACleanup is used to terminate the use of the WS2_32 DLL.
        return 1;
      }
      else
        cout<<"Socket Created SUCCESSFULLY with Socket ID: "<<server.getSocketID()<<endl; 


      //Step 3: Initialise the environment for sockaddr structure 
      // and Setup the TCP listening socket, i.e. BIND Socket to the local port( having port no: PORT ) on localhost 
      //Or Bind to the listener

      //The bind function (winsock.h) associates a local address with a socket. 
      
      const char * ip=""; 
      server.bindToSocket(PORT,ip);
      if(nRet == SOCKET_ERROR)
      {
        cout<<"FAILED to bind Socket to the local port "<<endl;
        wprintf(L"bind failed with error %u\n", WSAGetLastError());
        closesocket(server.getSocketID());
        WSACleanup();
        exit(EXIT_FAILURE);
      }
      else
        cout<<"BIND is SUCCESSFUL"<<endl;


      //Step 5: Listen to the  incoming requests from client, maximum 5 client requests
    
      nRet = server.startListen(5);
      if(nRet == SOCKET_ERROR) //-1
      {
        cout<<"FAILED to start listen listen on the local port "<<endl;
        exit(EXIT_FAILURE);
      }
      else
        cout<<"SUCCESS: Started to listen on the local port"<<endl; 

      //Create vector to store Client Sockets and their names
      // vector<pair<SOCKET,string>> clientInfo(MAX_CLIENTS); 
      // fd_set read_fd_set;
      // server.acceptAndProcessTheRequests(clientInfo,read_fd_set);

       //make 2 threads 1. for accepting the new requests 2. for receiving data on existing sockets
        
        thread acceptRequest(handleAcceptRequest,server.getSocketID());
        acceptRequest.detach();
        thread receiveRequest(handleSendRequest,server.getSocketID());
        receiveRequest.join();


    //close listener socket 
      closesocket(server.getSocketID());
      WSACleanup();
      return 0;
}
