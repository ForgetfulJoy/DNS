#ifndef CACHE_H
#define CACHE_H

#include <time.h>
#include <string>
#include <map>
#include <fstream>
#include "socket.h"
#include <iostream>

const int MAX_CACHE_SIZE = 100;

class Cache{
public:
    std::map<std::string, time_t> cacheList;
    std::map<std::string, unsigned int> nameList;
    std::ofstream fout;
    std::ifstream fin;

    Cache();
    ~Cache();
    bool updateTime(time_t time, std::string name);
    void getIP(std::string name, unsigned int &ip);
    void insertName(std::string name, unsigned int ip, time_t time);
};

#endif // CACHE_H