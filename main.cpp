#include "packet.h"
#include "socketop.h"
#include "record.h"
#include "cache.h"
#include <map>
#include <iostream>
#include <fstream>
#include <cstdio>

socketop *op;
sockaddr_in server_addr;
Packet packet;
Record record_table;
Cache *cache;
unsigned char *buffer;
int bufsize;
std::map<std::string, unsigned int> nameTable;

void initTable() {
    std::ifstream infile;
    infile.open("dnsrelay.txt");
    if (infile.is_open()) {
        while (!infile.eof()) {
            std::string ip;
            std::string domainname;
            infile >> ip >> domainname;
            in_addr ip_addr;
            inet_pton(AF_INET, ip.c_str(), (void *)&ip_addr);
            nameTable.insert(std::pair<std::string, unsigned int>(domainname, ip_addr.s_addr));
        }
    }
    infile.close();
}

void receive() {
    int len;
    sockaddr_in clientAddr;
    op->recvPacket(buffer, clientAddr, len);
    std::cout << std::endl << "-----------------------------------*/packet/*-----------------------------------" << std::endl;
    for (int i = 0; i < 100; i++) {
        printf("%0*X", sizeof(buffer[i])*2, buffer[i]);
        std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "--------------------------------------*//*--------------------------------------" << std::endl;
    bufsize = len;
    packet = Packet(clientAddr);
    unsigned char *pbuf = buffer;
    packet.getHeader(pbuf);
    packet.getQuery(pbuf);
    packet.getResource(pbuf, packet.ans, packet.header.ancount);
    packet.getResource(pbuf, packet.authority, packet.header.nscount);
    packet.getResource(pbuf, packet.add, packet.header.arcount); 
}

void recvServer() {
    struct record r;
    if (!record_table.returnID(buffer, r)) {
        std::cout << std::endl << "------** Record not found **------" << std::endl;
        exit(0);
    } else {
        unsigned short *id = (unsigned short *)buffer;
        *id = htons(r.id);
        time_t now = time(NULL);
        std::vector<Resource>::iterator iter = packet.ans.begin();
        for (; iter != packet.ans.end(); iter++) {
            std::string name = iter->rname;
            unsigned int ip;
            unsigned char *ipPtr = (unsigned char *)&ip;
            std::vector<unsigned char>::iterator i = iter->rdata.begin();
            for (; i != iter->rdata.end(); i++) {
                *ipPtr = *i;
                ipPtr++;
            }
            std::string domainname;
            packet.getDomainname(domainname, name);
            cache->insertName(domainname, ip, now);
        }
        if (sendto(op->hostSocket, buffer, bufsize, 0, (const sockaddr *)&r.clientAddr, sizeof(sockaddr_in)) < 0) {
            std::cout << std::endl << "------** Receive from server failed **------" << std::endl;
            exit(0);
        } else {
            std::cout << std::endl << "------** Receive from server success **------" << std::endl;
        }
    }
}

void sendToserver() {
    record_table.changeID(buffer, packet.send_addr);
    if (sendto(op->hostSocket, buffer, bufsize, 0, (const sockaddr *)&server_addr, sizeof(sockaddr_in)) < 0) {
        std::cout << std::endl << "------** Send to server failed **------" << std::endl;
        exit(0);
    } else {
        std::cout << std::endl << "------** Send to server success **------" << std::endl;
    }
}

void makeAns(unsigned int ip, std::string name) {
    if (ip == (unsigned int)0x00000000) {
        packet.header.rcode = 3;
    } else {
        packet.header.qr = true;
        packet.header.aa = true;
        packet.header.ra = true;
        packet.header.ancount++;
        Resource tmp_ans;
        tmp_ans.rname = name;
        tmp_ans.rtype = 1;
        tmp_ans.rclass = 1;
        tmp_ans.rlength = 4;
        tmp_ans.rtime = 120;
        tmp_ans.rdata.assign((unsigned char *)&ip, (unsigned char *)(&ip + 1));
        packet.ans.push_back(tmp_ans);
    }
}

void doQuery() {
    bool toServer = false;
    time_t now;
    std::vector<Query>::iterator i = packet.query.begin();
    for (; i != packet.query.end(); i++) {
        if ((i->qtype == 1) && (i->qclass == 1)) {
            std::string tmp_name;
            packet.getDomainname(tmp_name, i->qname);
            std::cout << std::endl << "Domain name: " << tmp_name << std::endl;
            now = time(NULL);
            bool is_local = false;
            if (cache->updateTime(now, tmp_name)) {
                unsigned int ip;
                cache->getIP(tmp_name, ip);
                makeAns(ip, i->qname);
            } else {
                std::map<std::string, unsigned int>::iterator index = nameTable.begin();
                for (; index != nameTable.end(); index++) {
                    if (index->first == tmp_name) {
                        makeAns(index->second, i->qname);    //Pay attention to the parameter
                        is_local = true;
                        std::cout << std::endl << "------** Find in the local list **------" << std::endl;
                        if (index->second == 0x00000000) {
                            std::cout << std::endl << "------** Shield IP **------" << std::endl;
                        }
                        cache->insertName(tmp_name, index->second, now);
                        break;
                    }
                }
                if (!is_local) {
                    std::cout << std::endl << "------** Not found in the local list **------" << std::endl;
                    sendToserver();
                    toServer = true;
                    break;
                }
            }
        } else {
            std::cout << std::endl << "------** Query is unacceptable **------" << std::endl;
            sendToserver();
            return;
        }
    }
    if (!toServer) {
        op->sendPacket(packet);
    }
}



int main() {
    op = new socketop();
    buffer = new unsigned char[4096];
    cache = new Cache();
    initTable();
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (!inet_pton(AF_INET, SERVER_IP.c_str(), (void *)&(server_addr.sin_addr))) {
        std::cout << std::endl << "------** Server IP wrong **------" << std::endl;
        exit(0);
    }

    while (1) {
        receive();
        if (packet.header.qr == 1) {
            std::cout << std::endl << "--------</* Answer mode */>--------" << std::endl;
            recvServer();
        } else if((packet.header.qr == 0) && (packet.header.opcode == 0)) {
            std::cout << std::endl << "--------</* Query mode */>--------" << std::endl;
            doQuery();
        } else {
            std::cout << std::endl << "--------</* Other */>--------" << std::endl;
            sendToserver();
        }
        std::cout << std::endl << std::endl;
    }

    return 0;
}


