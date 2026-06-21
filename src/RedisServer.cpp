#include "../include/RedisServer.h"
#include <iostream>
// #include<sys/socket.h> this is for linux not windows
#include <winsock2.h> // windows equivalent for socket API's
#include<unistd.h> // also for linux but mingw provides, its for POSIX programming
//#include<netinet/in.h> // for linux, used to tell machine about network which IP family, which port etc
#include <ws2tcpip.h> // windows eq of netinet

static RedisServer* globalServer = nullptr;


// : is a member initializer list 
// :: is scope resolution operator
RedisServer::RedisServer (int port) : port(port), server_socket(-1), running(true){
    globalServer = this; // current object
};

void RedisServer::shutdown (){
    running = false;

    if(server_socket != -1){
        close(server_socket);
    }

    std::cout<< "Server shutdown complete! \n";
};

void RedisServer::run(){
    // Create a TCP socket.
    // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = use the default protocol for TCP.
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket<0) {
        std::cerr<< "Error creating socket\n";
        return;
    }

    int opt=1;
    // changing the option SO_REUSEADDR to true so the server can rebind to a recently-used IP address and port without waiting for old TCP connections in TIME_WAIT to fully expire.
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&opt), sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family=AF_INET;
    // htons() converts the port from host byte order to network byte order.
    serverAddr.sin_port= htons(port);
    // listen to all the IP this machine has
    // to constraint it to only local host use inet_addr("127.0.0.1")
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // bind takes the socket, the address info and its size
    // make a pointer pointing to the memory of sockaddr
    if(bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))<0){
        std::cerr<<"Error binding to Server Socket\n";
        return;
    }

    if(listen(server_socket, 10)<0){
        std::cerr<<"Error Listening on Server socket\n";
        return;
    }

    std::cout<<"Server listening on port "<<port<<"\n";

};

