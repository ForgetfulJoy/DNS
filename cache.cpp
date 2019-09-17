#include "cache.h"
#include <algorithm>
#include <vector>

Cache::Cache() {
    fout.open("cache.txt");
    fin.open("cache.txt");
    if (fin.is_open()) {
        time_t now;
        now = time(NULL);
        while(!fin.eof()) {
            std::string domainname;
            std::string ip;
            fin >> ip >> domainname;
            in_addr ip_addr;
            inet_pton(AF_INET, ip.c_str(), (void *)&ip_addr);
            nameList.insert(std::pair<std::string, unsigned int>(domainname, ip_addr.s_addr));
            cacheList.insert(std::pair<std::string, time_t>(domainname, now));
        }
    } else {
        std::cout << std::endl << "------** Cache open failed **------" << std::endl;
    }
}

Cache::~Cache() {
    fout.close();
    fin.close();
}

bool Cache::updateTime(time_t time, std::string name) {
    std::map<std::string, time_t>::iterator i = cacheList.find(name);
    if (i != cacheList.end()) {
        i->second = time;
        std::cout << std::endl << "------** Find in cache **------" << std::endl;
        return true;
    } else {
        std::cout << std::endl << "------** Not found in cache **------" << std::endl;
    }
    return false;
}

void Cache::getIP(std::string name, unsigned int &ip) {
    std::map<std::string, unsigned int>::iterator i = nameList.find(name);
    if (i != nameList.end()) {
        ip = i->second;
    } 
}

bool cmp(const std::pair<std::string, time_t> &p1, const std::pair<std::string, time_t> &p2) {
    return p1.second < p2.second;
}

void Cache::insertName(std::string name, unsigned int ip, time_t time) {
    nameList.insert(std::pair<std::string, unsigned int>(name, ip));
    std::map<std::string, unsigned int>::iterator i = nameList.begin();
    i++;
    char str[16];
    inet_ntop(AF_INET, (void *)&(ip), str, 16);
    fout << std::string(str) << " " << name << std::endl;
    std::cout << std::endl << "------** Write a new record to cache **------" << std::endl;
    cacheList.insert(std::pair<std::string, time_t>(name, time));
    if (nameList.size() > MAX_CACHE_SIZE) {
        std::cout << std::endl << "------** The cache is full, change one record **------" << std::endl;
        std::vector<std::pair<std::string, time_t> > sortList;
        std::map<std::string, time_t>::iterator iter = cacheList.begin();
        for (; iter != cacheList.end(); iter++) {
            sortList.push_back(std::pair<std::string, time_t>(iter->first, iter->second));
        }
        std::sort(sortList.begin(), sortList.end(), cmp);
        std::string tmp_name = sortList.begin()->first;
        iter = cacheList.find(tmp_name);
        cacheList.erase(iter);
        std::map<std::string, unsigned int>::iterator i = nameList.find(tmp_name);
        nameList.erase(i);
        i = nameList.begin();
        fout.close();
        fout.open("cache.txt", std::ios::trunc);
        for (; i != nameList.end(); i++) {
            char str[16];
            inet_ntop(AF_INET, (void *)&(i->second), str, 16);
            fout << std::string(str) << " " << i->first << std::endl;
        }
    }
}