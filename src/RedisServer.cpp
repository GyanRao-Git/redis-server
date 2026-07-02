#include "../include/RedisServer.h"
#include "../include/RedisCommandHandler.h"
#include "../include/RedisDatabase.h"
#include "../include/Config.h"
#include <iostream>
// #include<sys/socket.h> this is for linux not windows
#include <winsock2.h> // windows equivalent for socket API's
#include<unistd.h> // also for linux but mingw provides, its for POSIX programming
//#include<netinet/in.h> // for linux, used to tell machine about network which IP family, which port etc
#include <ws2tcpip.h> // windows eq of netinet
#include<vector>
#include<thread>
#include<cstring>
#include <signal.h>

static RedisServer* globalServer = nullptr;

void signalHandler(int signum){
    if(globalServer){
        std::cout<< "Caught Signal "<< signum << " Shutting down ... \n";
        globalServer->shutdown();
    }
    exit(signum);
}

void RedisServer::setupSignalHandler(){
    signal(SIGINT, signalHandler);
}



// : is a member initializer list
// :: is scope resolution operator
RedisServer::RedisServer (int port) : port(port), server_socket(-1), running(true){
    globalServer = this; // current object
    setupSignalHandler();
};

void RedisServer::shutdown (){
    running = false;

    if(server_socket != -1){
        close(server_socket);
    }

    std::cout<< "Server shutdown complete! \n";
};

void RedisServer::run(){

    //Need to initilize WSA
    //WSA = windows socket APIS
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return;
    }

    // Create a TCP socket.
    // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = use the default protocol for TCP.
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket<0) {
        int err = WSAGetLastError();
        std::cerr<< "Error creating socket: "<< err << std::endl;;
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

    std::vector<std::thread> threads;

    // object
    RedisCommandHandler cmdHandler;

    while(running){
        int client_socket = accept(server_socket, nullptr, nullptr);
        if(client_socket<0){
            if(running){
                std::cerr << "Client Socket connection failed! \n";
            }else{
                std::cerr<<"Socket closing so client connection rejected \n";
            }
            break;
        }

        // emplace back constructs an object and passes the values to the functions params
        // here the lambda is passes to the constructor and the whole object is pushed back into the vector
        threads.emplace_back([client_socket, &cmdHandler]() {
            char buffer[1024];
            while(true){
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
                // std::cout<<"[DEUBG] "<<buffer<<std::endl;
                if(bytes==0){
                    //No more data will ever come from here
                    //TCP Fin packet is 0
                    std::cout<<"Client disconnected \n";
                    close(client_socket);
                    break;
                }   
                else if(bytes < 0){
                    //caused by network failure, invalid socket..etc
                    perror("recv");
                    break;
                }

                std::string request(buffer, bytes);
                // std::cout<< "[REQUEST] "<< request<<'\n';
                std::string response = cmdHandler.processCommand(request);
                send(client_socket, response.c_str(), bytes, 0);
            }
            close(client_socket);
        });
        
        for(auto &t:threads){
            if(t.joinable()){
                t.join();
            }
        }
    }

    // Before shutting down persist the db. Note: 'fileName' is not defined in this snippet.
    RedisDatabase::dumpIntoDb(fileName);
};
