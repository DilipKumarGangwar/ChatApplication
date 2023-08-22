//Client Program
#include<iostream>
#include<winsock2.h>
#include<string>
#include "../Headers/Client.h"

#include<thread>
using namespace std;

#define PORT 9909
#define DATA_BUFFER 1024
struct sockaddr_in  srv;
string clientName;

Client::Client()
{
   listenSocket = INVALID_SOCKET;
}

Client::~Client()
{
  if (listenSocket != INVALID_SOCKET) 
            closesocket(listenSocket);
   
    WSACleanup();
}

int Client::initializeWSA()
{
    WSADATA ws;
    if(WSAStartup(MAKEWORD(2,2),&ws)<0)
        exit(EXIT_FAILURE);
    else      
       return EXIT_SUCCESS;    
}

void Client::createClientSocket()
{
    listenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
}

SOCKET Client::getSocketID()
{
    return listenSocket;
}

int Client::connectToServer(const char *ip,int port )
{
    srv.sin_family=AF_INET;
    srv.sin_port = htons(port); 
    srv.sin_addr.s_addr =  inet_addr(ip);
    memset(&(srv.sin_zero),0,sizeof(srv.sin_zero));

   //connect to server
   return connect(listenSocket,(struct sockaddr*)&srv,sizeof(srv));
   
}
int Client::sendName(string clientName)
{
    return send(listenSocket,clientName.c_str(),strlen(clientName.c_str())+1,0); //send client name to server 
}



void CleanUp(SOCKET clientSocket)
{
    closesocket(clientSocket);
    WSACleanup();
}

void handleSend(SOCKET clientSocket)
{
  string message;
  do
  {
        std::cout<<clientName + ":";
        getline(cin,message);
        // Include client's name in the message
        message = clientName + ": " + message;

        int nStatus = send(clientSocket,message.c_str(),strlen(message.c_str())+1,0);
   
        //nStatus gets the no of bytes sent actually
        if(nStatus == SOCKET_ERROR)
        {
            std::cout<<"Send Data FAILED "<<WSAGetLastError()<<endl;
            CleanUp(clientSocket);
            exit(EXIT_FAILURE);
        }  
        cout<<"Message Sent"<<endl;
  } while (1);
  
}

void handleReceive(SOCKET clientSocket)
{
   char buffer[DATA_BUFFER]; 
   memset(buffer,0,DATA_BUFFER);
   while(1)
   {
        int bytesRead = recv(clientSocket, buffer, DATA_BUFFER-1, 0);
        if (bytesRead == -1)
        {
            std::cout << "Recv failed!" << std::endl;
            CleanUp(clientSocket);
            exit(EXIT_FAILURE);
        }
        else if (bytesRead == 0) 
        {
            std::cout << "Connection closed by remote side/server." << std::endl;
            break;
        } 
        else
        {
            // Process the received data in 'buffer'
            buffer[bytesRead] = '\0'; // Ensure null-termination
            std::cout << '\r';
            std::cout << buffer << "\n";
            //print the prompt again
            std::cout << clientName << ": ";
        }
   }
}


int main(int argc, char* argv[])
{
   
    //Create Client Object
    Client client;
     
    //Initialise the WSA Variables
    if(client.initializeWSA())  
        exit(EXIT_FAILURE);          

    int nRet = 0;
    //initialise the Socket
    client.createClientSocket();
    if(client.getSocketID() == INVALID_SOCKET)
    {
       std::cout<<"Socket Creation FAILED: Error at Socket "<<WSAGetLastError();
       WSACleanup(); // WSACleanup is used to terminate the use of the WS2_32 DLL.
       return 1;
    }
    else
       std::cout<<"Socket Created SUCCESSFULLY with Socket ID: "<<client.getSocketID()<<endl; 

    //Initialise the environment for sockaddr structure
    //and connect to server
   const char * ip="192.168.43.208"; 
   int nStatus = client.connectToServer(ip,PORT);
   if(nStatus < 0)
   {  
      std::cout<<"FAILED: failed to connect"<<endl;
      exit(EXIT_FAILURE);
   }

   //if connect is successful
   //send first the name of Client and then your data to server

   //sending name of client to server
    cout<<"Hi,What's you name";
    getline(cin,clientName);
    nStatus = client.sendName(clientName); 

    //nStatus gets the no of bytes sent actually
    if(nStatus < 0)
    {
        std::cout<<"Sending CLIENT NAME  FAILED "<<endl;
        exit(EXIT_FAILURE);
    }  
 
   

   
   thread senderThread(handleSend,client.getSocketID());
   senderThread.detach();

   thread receiverThread(handleReceive,client.getSocketID());
   receiverThread.join();
   

  // std::cout<<"Client Disconnected from server";

    return 0;
}