#ifndef RECORD_H
#define RECORD_H

#include "socket.h"
#include "packet.h"


class Record {
public:
    std::vector<record> recordTable;
    int num;
    Record();
    void changeID(unsigned char *buf, sockaddr_in clientAddr);
    bool returnID(unsigned char *buf, struct record &r);
};

#endif // RECORD_H