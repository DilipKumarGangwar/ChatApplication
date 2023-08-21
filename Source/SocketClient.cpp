//Client Program
#include<iostream>
#include<winsock2.h>
#include<D:\Interviews\Project\Client_Server_Chat_Application_Final\Headers\Client.h>
using namespace std;

#define PORT 9909
#define DATA_BUFFER 1024
struct sockaddr_in  srv;

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
int Client::sendName(const char * name)
{
    return send(listenSocket,name,DATA_BUFFER,0); //send client name to server 
}

int Client::sendData(const char* data)
{
    return send(listenSocket,data,DATA_BUFFER,0); //send client name to server
}


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <client_name>" << endl;
        return 1;
    }
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
   const char * ip="192.168.43.254"; 
   int nStatus = client.connectToServer(ip,PORT);
   if(nStatus < 0)
   {  
      std::cout<<"FAILED: failed to connect"<<endl;
      exit(EXIT_FAILURE);
   }

   //if connect is successful
   //send first the name of Client and then your data to server

   //sending name of client to server
    const char *clientName = argv[1];
    nStatus = client.sendName(clientName); 

    //nStatus gets the no of bytes sent actually
    if(nStatus < 0)
    {
        std::cout<<"Sending CLIENT NAME  FAILED "<<endl;
        exit(EXIT_FAILURE);
    }  
 
   //Send your data now
   char buffer[DATA_BUFFER];

   fd_set readfds;
    FD_ZERO(&readfds);
  
// Initialize the timer
    time_t timerStart = 0;
    const int messageInterval = 1; // Wait time in seconds
   do
   {
      FD_SET(client.getSocketID(), &readfds);
      struct timeval timeout;
      timeout.tv_sec = messageInterval;
      timeout.tv_usec = 0;     
        std::cout<<endl<<"Please enter the message to send to Server: ";
        cin.getline(buffer,200);
        // Include client's name in the message
        string message = string(clientName) + ": " + buffer;

        nStatus = client.sendData(message.c_str());
    // memset(buffer, 0, DATA_BUFFER); // Clear the buffer
        //nStatus = client.sendData(buffer); //send data
        //nStatus gets the no of bytes sent actually
        if(nStatus < 0)
        {
            std::cout<<"Send Data FAILED "<<endl;
            exit(EXIT_FAILURE);
        }  
        else if(nStatus >0)
        {
            // Start the timer
                timerStart = time(nullptr);

                // Use select to wait for either data from the socket or the timer
                int result = select(client.getSocketID() + 1, &readfds, nullptr, nullptr, &timeout);
                if (result == -1) {
                    std::cerr << "Select faailed!" << std::endl;
                } else if (result == 0) {
                    std::cout << "Timer elapsed, please enter another message." << std::endl;
                } else {
                    if (FD_ISSET(client.getSocketID(), &readfds)) {
                        int bytesRead = recv(client.getSocketID(), buffer, sizeof(buffer), 0);
                        if (bytesRead == -1) {
                            std::cerr << "Recv failed!" << std::endl;
                        } else if (bytesRead == 0) {
                            std::cout << "Connection closed by remote side/server." << std::endl;
                            break;
                        } else {
                            // Process the received data in 'buffer'
                            buffer[bytesRead] = '\0'; // Ensure null-termination
                            std::cout << "Received: " << buffer << std::endl;
                        }
                    }
                }//else
        }

   } while(strcmp(buffer,"bye"));

   std::cout<<"Client Disconnected from server";

    return 0;
}