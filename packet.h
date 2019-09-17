#ifndef PACKET_H
#define PACKET_H

#include "packetform.h"
#include "socket.h"

class Packet {
public:
    Header header;
    sockaddr_in send_addr;
    std::vector<Query> query;
    std::vector<Resource> ans;
    std::vector<Resource> authority;
    std::vector<Resource> add;

    void getHeader(unsigned char *&header_p);
    void getQuery(unsigned char *&buf);
    void getResource(unsigned char *&buf, std::vector<Resource> &buf_rsc, int rcount);
    void getDomainname(std::string &buf, std::string name);

    void byteTobuf2(unsigned short v, unsigned char *&buf, int &bufsize);
    void byteTobuf4(unsigned int v, unsigned char *&buf, int &bufsize);
    void setHeader(unsigned short &headerbuf);
    void queryTobuf(unsigned char *&buf, int &bufsize);
    void resourceTobuf(std::vector<Resource> r, unsigned char *&buf, int &bufsize);

    Packet();
    Packet(const sockaddr_in send_addr);
};

#endif // PACKET_H