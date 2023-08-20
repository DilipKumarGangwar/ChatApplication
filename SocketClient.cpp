//Client Program

#include<iostream>
#include<winsock2.h>

using namespace std;

#define PORT 9909
struct sockaddr_in  srv;



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
    srv.sin_addr.s_addr =  inet_addr("192.168.43.208");
   // memset(&(srv.sin_zero),0,8);

    memset(&(srv.sin_zero),0,sizeof(srv.sin_zero));

   //connect
   int nStatus = connect(listenSocket,(struct sockaddr*)&srv,sizeof(srv));
   if(nStatus < 0)
   {  
      cout<<"FAILED: failed to connect"<<endl;
      exit(EXIT_FAILURE);
   }

   //if connect is successful
   //send data to server
   char buffer[200];
   do
   {
    
    cout<<endl<<"Please enter the message to send to Server: ";
    cin.getline(buffer,200);
    nStatus = send(listenSocket,buffer,200,0); //send data
    //nStatus gets the no of bytes sent actually
    if(nStatus < 0)
    {
        cout<<"Send Data FAILED "<<endl;
        exit(EXIT_FAILURE);
    }  
    else if(nStatus >0)
    {
        cout<<"Message Sent"<<endl;
    }

   } while(strcmp(buffer,"bye"));

    return 0;
}