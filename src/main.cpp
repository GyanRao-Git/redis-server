#include "../include/RedisServer.h"
#include <iostream>

#define PORT 6973

int main(int argc, char * argv[]){
    int port = PORT;

    //   ./redis-server 6379 <- port
    if(argc>=2) port =  std::stoi(argv[1]);

    RedisServer server(port);

    return 0;
}