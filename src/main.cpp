#include "../include/RedisServer.h"
#include <iostream>
#include <thread>
#include <chrono>

#define PORT 6973

int main(int argc, char * argv[]){
    int port = PORT;

    //   ./redis-server 6379 <- port
    if(argc>=2) port =  std::stoi(argv[1]);

    RedisServer server(port);

    //dump to the db every 5 mins (seperate thread cause main is busy handling clients)
    std::thread persistanceThread([](){
        while(true){
            std::this_thread::sleep_for(std::chrono::seconds(300));
            //dump the db
        }   
    });

    // not managed by std::thread, OS owns it now
    // main thread doesnt wait for it to finish
    persistanceThread.detach();

    server.run();

    return 0;
}