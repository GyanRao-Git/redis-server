#include "../include/RedisCommandHandler.h"
#include "../include/RedisDatabase.h"
#include <vector>
#include <sstream>
#include<algorithm>
#include <iostream>

// RESP parser:
// *2\r\n$4\r\n\PING\r\n$4\r\nTEST\r\n
// *2 -> array has 2 elements
// $4 -> next string has 4 characters
// PING
// TEST

std::vector<std::string> parseNonResp (const std::string input){
    std::vector<std::string> ans;
    std::istringstream iss(input);

    std::string token;
    while(iss>>token){
        ans.push_back(token);
    }
    return ans;
}

std::vector<std::string> parseResp (const std::string &input){
    std::vector<std::string> tokens;
    if(input.empty()) return tokens;

    // redis client comands are always sent as array, if not then not RESP
    if(input[0] != '*'){
        tokens = parseNonResp(input);
        return tokens;
    }

    size_t pos = 1; //skip the *

    // crlf = Carriage Return (\r), Line Feed (\n)
    size_t crlf = input.find("\r\n", pos);
    //invalid -> RESP will always contain at the end
    if (crlf == std::string::npos) return tokens;

    int numElements = 0;
    std::string w = input.substr(pos, crlf-pos);

    try{
        int temp = std::stoi(w);
        numElements = temp;
    }
    catch(const std::invalid_argument& e){
        // not a valid number
        return tokens;
    }
    catch (const std::out_of_range& e) {
        // too long number, out of range, runtime err
        return tokens;
    }

    pos = crlf+2;

    while(numElements--){
        if(pos>=input.size() || input[pos]!='$') break; // format error, should be a number

        pos++; // skip $ ->next is number

        crlf = input.find("\r\n", pos);
        if(crlf == std::string::npos) break;

        int len = std::stoi(input.substr(pos, crlf - pos));
        pos = crlf + 2;

        std::string token = "";
        while(len--){
            if(pos>=input.size()) break;
            token += input[pos++];
        }
        tokens.push_back(token);
        pos +=2 ; // skip crlf
    }
    // for(auto s:tokens){
    //     std::cout<<"Tokens"<<'\n';
    //     std::cout<<"->"<<s<<" ";
    //     std::cout<<"\n";
    // }
    
    return tokens;
};

RedisCommandHandler::RedisCommandHandler(){
    //pass
};


std::string RedisCommandHandler::processCommand(const std::string& commandLine){
    auto tokens = parseResp(commandLine);
    if(tokens.empty()) return "-ERR: Empty Command\r\n";

    std::string cmd = tokens[0];
    std::transform(cmd.begin(),cmd.end(), cmd.begin(), ::toupper);

    std::ostringstream res;

    RedisDatabase& db = RedisDatabase::getInstance();

    // Check commands

    //Common commands
    if (cmd == "PING") {
        // std::cout<<"got ping"<<'\n';
        res << "+PONG\r\n";
    }
    else if (cmd == "ECHO") {
        if(tokens.size()<2){
            std::cout<<"got a wrong echo\n";
            res<< "-ERR: ECHO rquires a message \r\n";
        }else{
            res <<"+" << tokens[1]<<"\r\n";
        }
    }else if(cmd == "FLUSHALL"){
        // db.flushAll();
        res << "+OK\r\n";
    }    

    // key/value operations
    else if(cmd == "SET"){
        if(tokens.size()<3){
            res<<"SET command needs a key and a value\r\n";
        }else{
            db.set(tokens[1], tokens[2]);
            res<<"+OK\r\n";
        }
    }

    std::string response = res.str();
    return response;
}