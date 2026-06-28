#ifndef REDIS_SERVER_H
#define REDIS_SERVER_H

#include <string>

// C++ library for variables that can be safely shared and modified between threads without data races.
#include<atomic> 

class RedisServer{

public:
    RedisServer(int port);
    void run();
    void shutdown();
    void setupSignalHandler();
private:
    int port;
    int server_socket;
    bool running;
};

#endif