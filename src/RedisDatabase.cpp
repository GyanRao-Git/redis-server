#include "../include/RedisDatabase.h"
#include<iostream>
#include<string>
#include <mutex>
#include<fstream>
#include<sstream>
#include<unordered_map>

// RedisDatabase::RedisDatabase(){
//     //pass
// }

RedisDatabase& RedisDatabase::getInstance(){
    static RedisDatabase instance;
    return instance;
}

//thread safe dumping into db
bool RedisDatabase::dump(const std::string &fileName){
    // lock is released when scope runs out
    std::lock_guard<std::mutex> lock (db_mutex);

    //output filestream, used to write data to a file
    //no need to call clsoe(), as it is RAII, destructor closes it
    std::ofstream ofs(fileName, std::ios::binary);
    if(!ofs){
        std::cout<< "Failed to open the file: "<< fileName << '\n';
        return false;
    } 

    //duping kv store
    if(!kv_store.empty()){
        for(const auto & key_value: kv_store){
            ofs << "K "<< key_value.first << " " <<key_value.second<< '\n';
        }
    }

    //dumping list
    if(!list_store.empty()){
        for(const auto& key_list:list_store){
            ofs << "L " << key_list.first;
            for(const auto&s: key_list.second) ofs << " "<< s;
            ofs <<'\n';
        }
    }

    // dumping hash

    if(!hash_store.empty()){
        for(const auto &[s,hash]:hash_store ){
            ofs << "H " << s;
            for(const auto&[key,val]:hash){
                ofs<< " " << key << ":" << val << " ";
            }
            ofs <<'\n';
        }
    }



    return true;
}

/*
    Key-Value (K)
    kv_store["name"] = "Alice";
    kv_store["city"] = "Berlin";

    List (L)
    list_store["fruits"] = {"apple", "banana", "orange"};
    list_store["colors"] = {"red", "green", "blue"};

    Hash (H)
    hash_store["user:100"] = {
        {"name", "Bob"},
        {"age", "30"},
        {"email", "bob@example.com"}
    };

    hash_store["user:200"] = {
        {"name", "Eve"},
        {"age", "25"},
        {"email", "eve@example.com"}
    };
*/

bool RedisDatabase::load(const std::string &fileName){
    std::lock_guard<std::mutex> lock(db_mutex);
    std::ifstream ifs(fileName, std::ios::binary);

    if(!ifs){
        std::cout<<"Error reading from the file: "<<fileName<<'\n';
        return false;
    }

    kv_store.clear();
    list_store.clear();
    hash_store.clear();

    std::string line;
    while(std::getline(ifs, line)){
        std::istringstream iss(line);
        char type;
        iss >> type;
        if(type == 'K'){
            std::string key,val;
            iss>>key>>val;
            kv_store[key] = val;
        }
        else if(type=='L'){
            std::string key,temp;
            std::vector<std::string> list;
            iss >> key;
            while(iss>>temp){
                list.push_back(temp);
            }
            list_store[key]=list;
        }else if(type=='H'){
            std::string mainKey,key_value;
            std::unordered_map<std::string, std::string> mpp;
            iss >> mainKey;
            while(iss>>key_value){
                std::string key,value;
                int pos = key_value.find(":");
                if(pos == std::string::npos){
                    std::cout<<"Invalid key stored: "<<key_value<<'\n';
                    return false;
                }
                key = key_value.substr(0,pos);
                value = key_value.substr(pos+1);
                mpp[key]=value;
            }
            hash_store[mainKey] = mpp;

        }

    }

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

