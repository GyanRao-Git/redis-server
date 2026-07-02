#include "../include/RedisDatabase.h"
#include<iostream>
#include<string>


// RedisDatabase::RedisDatabase(){
//     //pass
// }

RedisDatabase& RedisDatabase::getInstance(){
    static RedisDatabase instance;
    return instance;
}

bool RedisDatabase::dump(const std::string &fileName){
    return true;
}

bool RedisDatabase::load(const std::string &fileName){
    return true;
}

bool RedisDatabase::dumpIntoDb(const std::string &fileName){
    //Gets instance and dumps it WITH error handling
    RedisDatabase &db = RedisDatabase::getInstance();
    if(!db.dump(fileName)){
        std::cerr << "Error dumping db \n";
    }
    else{
        std::cout << "Data dumped into " << fileName << '\n';
        return true;
    }
    return false;
}   

