# ChatApplication
**SocketServer.cpp**
This is Server Source Code.
1. Server can listen to multiple clients requests and messages
2. Server can Stop by pressing Ctrl+c on command prompt

**SocketClient.cpp**
This is Client Source Code.
1.Client can send the message to servers and gets the same message as echoed from server
2.Clients can disconnect from server by pressing ctrl+C on client cmd
3.User can open multiple instances of client in different cmd

**Server Remains open even if all clients disconnects from it.**
Server will stop only when user explicitly stops by Pressing ctrl + c

**How to Use**
1. Download or clone the code in your local system.
2. Compile Server and client code
   Compile Client Code by writing this command: g++ SocketClient.cpp -o client -lwsock32  (you can give any name for your executable after -o, here I have given name as client)
   Compile Server Code by writing this command: g++ SocketServer.cpp -o server -lwsock32  (you can give any name for your executable after -o, here I have given name as server)
3. Now Open a cmd
4. Go to the source folder path and Run the server first by typing this command: ./server.exe
5. Now Open an new cmd.
6. Go to the source folder path and Run the client by typing this command: ./client.exe <any name of the client> e.g: ./client.exe Dilip
   (Giving the name of client is mandatory)
8. On this client cmd: Now you can type you message and it will be visible on server cmd prompt
   The same message will be send back by server to you and it will be visible on your client cmd prompt also
10. You can stop the client connection by pressing ctrl+c
11. Same way you can stop the server by pressing ctrl+c
12. You can parallelly open a new client cmd prompt ( without closing the first client cmd) and start new client connection by typing  ./client.exe <any name of the client> e.g: ./client.exe Ramesh
13. Remember you have to  go to first the source folder path always
