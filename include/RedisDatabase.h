#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H

#include<string>

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

};

#endif