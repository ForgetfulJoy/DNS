#ifndef PACKETFORM_H
#define PACKETFORM_H

#include "stdint.h"
#include "stdbool.h"
#include "socket.h"
#include <string>
#include <vector>

struct Header {
//       0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15 
//      ------------------------------------------------------
//      |                         ID                         |
//      ------------------------------------------------------
//      |QR|   OPCODE  |AA|TC|RD|RA|  (zero)  |    rcode     |
//      ------------------------------------------------------
//      |                      QDCOUNT                       |
//      ------------------------------------------------------
//      |                      ANCOUNT                       |
//      ------------------------------------------------------
//      |                      NSCOUNT                       |
//      ------------------------------------------------------
//      |                      ARCOUNT                       |
//      ------------------------------------------------------

    unsigned short id;
    bool qr;
    unsigned char opcode;
    bool aa;
    bool tc;
    bool rd;
    bool ra;
    unsigned char zero;
    unsigned char rcode;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

struct Query {
//       0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15
//      -----------------------------------------------------
//      |                                                   |
//      /                      QNAME                        /
//      |                                                   |
//      -----------------------------------------------------
//      |                      QTYPE                        |
//      -----------------------------------------------------
//      |                      QCLASS                       |
//      -----------------------------------------------------

    std::string qname;
    unsigned short qtype;
    unsigned short qclass;
};

struct Resource {
//       0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15
//      -----------------------------------------------------
//      |                                                   |
//      /                       RNAME                       /
//      |                                                   |
//      -----------------------------------------------------
//      |                       RTYPE                       |
//      -----------------------------------------------------
//      |                       RCLASS                      |
//      -----------------------------------------------------
//      |                       RTIME                       |
//      |                                                   |
//      -----------------------------------------------------
//      |                      RLENGTH                      |
//      -----------------------------------------------------
//      |                                                   |
//      /                       RDATA                       /
//      |                                                   |
//      -----------------------------------------------------

    std::string rname;
    unsigned short rtype;
    unsigned short rclass;
    unsigned int rtime;
    unsigned short rlength;
    std::vector<unsigned char> rdata;
};

struct record {
    unsigned short id;
    unsigned short number;
    sockaddr_in clientAddr;
};

#endif // PACKETFORM_H