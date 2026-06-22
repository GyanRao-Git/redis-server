#ifndef REDIS_COMMAND_HANDLER_H
#define REDIS_COMMAND_HANDLER_H

#include<string>

class RedisCommandHandler{
private:
    
public:
    RedisCommandHandler();

    //process a command from client and return in RESP format
    std::string processCommand(const std::string command);
};




#endif