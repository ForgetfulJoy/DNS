#include "record.h" 
#include <iostream>

Record::Record() {
    this->num = 0;
}

void Record::changeID(unsigned char *buf, sockaddr_in clientAddr) {
    num++;
    unsigned short *id = (unsigned short *)buf;
    record tmp = {
        .clientAddr = clientAddr,
        .number = num
    };
    tmp.id = ntohs(*id);
    std::cout << std::endl << "------** Record and send to server **------" << std::endl;
    std::cout << "--------*//*--------" << std::endl;
    std::cout << "Original ID " << tmp.id << std::endl;
    std::cout << "Change to " << tmp.number << std::endl;
    std::cout << "--------*//*--------" << std::endl;
    *id = htons(num);
    this->recordTable.push_back(tmp);
}

bool Record::returnID(unsigned char *buf, struct record &r) {
    unsigned short *n = (unsigned short *)buf;
    std::vector<record>::iterator i = recordTable.begin();
    for (; i != recordTable.end(); i++) {
        if (i->number == ntohs(*n)) {
            break;
        }
    }
    if (i != recordTable.end()) {
        r.id = i->id;
        r.clientAddr = i->clientAddr;
        r.number = i->number;
        std::cout << std::endl << "----------*//*----------" << std::endl;
        std::cout << "Return back to ID " << r.id << std::endl;
        std::cout << "----------*//*----------" << std::endl;
        recordTable.erase(i);
        return true;
    } else {
        return false;
    }
}