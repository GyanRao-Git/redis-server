#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H

#include<string>
#include <mutex>
#include <unordered_map>
#include<vector>

class RedisDatabase{
private:
    RedisDatabase() = default;
    ~RedisDatabase() = default;
    RedisDatabase(const RedisDatabase&) = delete;
    RedisDatabase &operator = (const RedisDatabase&) = delete;

public:
    // get the singleton instance
    static RedisDatabase &getInstance();

    // Persistance: Dump/Load the database from a file
    bool load(const std::string &filename);
    bool dump(const std::string &filename);

    static bool dumpIntoDb(const std::string &filename);

private:
    
    std::mutex db_mutex;   

    // kv_store [key]= value
    std::unordered_map<std::string, std::string> kv_store;
    std::unordered_map<std::string, std::vector<std::string>> list_store;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> hash_store;
};

#endif