#include "socketop.h"
#include <iostream>

socketop::socketop() {
    hostSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (hostSocket >= 0) {
        sockaddr_in host_addr;
        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(PORT);
        host_addr.sin_addr.s_addr = 0;
        if (bind(hostSocket, (sockaddr *)&host_addr, sizeof(sockaddr_in)) == -1) {
            std::cout << std::endl << "------** Binding error **------" << std::endl;
            exit(1);
        } else {
            std::cout << std::endl << "------** Binding success **------" << std::endl;
        }
    }
}

socketop::~socketop() {
    close(hostSocket);
}

void socketop::sendPacket(Packet &p) {
    unsigned char buf[MAX_PACKET];
    memset(buf, 0, MAX_PACKET);
    int bufsize = 0;
    unsigned char *tmp_buf = buf;
    p.byteTobuf2(p.header.id, tmp_buf, bufsize);
    unsigned short headerbuf = 0;
    p.setHeader(headerbuf);
    p.byteTobuf2(headerbuf, tmp_buf, bufsize);
    p.byteTobuf2((unsigned short)p.query.size(), tmp_buf, bufsize);
    p.byteTobuf2((unsigned short)p.ans.size(), tmp_buf, bufsize);
    p.byteTobuf2((unsigned short)p.authority.size(), tmp_buf, bufsize);
    p.byteTobuf2((unsigned short)p.add.size(), tmp_buf, bufsize);
    p.queryTobuf(tmp_buf, bufsize);
    p.resourceTobuf(p.ans, tmp_buf, bufsize);
    p.resourceTobuf(p.authority, tmp_buf, bufsize);
    p.resourceTobuf(p.add, tmp_buf, bufsize);
    tmp_buf = buf;
    if (sendto(hostSocket, tmp_buf, bufsize, 0, (const sockaddr *)&p.send_addr, sizeof(sockaddr_in)) < 0) {
        std::cout << std::endl << "------** Send back to client failed **------" << std::endl;
        exit(0);
    } else {
        std::cout << std::endl << "------** Send back to client success **------" << std::endl;
    }
}

void socketop::recvPacket(unsigned char *buf, sockaddr_in &sendAddr, int &len) {
    memset(buf, 0, 4096);
    socklen_t len_addr = sizeof(sockaddr_in);
    sockaddr_in clientAddr;
    int len_recv;
    len_recv = recvfrom(hostSocket, (char *)buf, MAX_PACKET, 0, (sockaddr *)&clientAddr, &len_addr);
    if (len_recv <= 0) {
        std::cout << std::endl << "------** Receive packet failed **------" << std::endl;
        exit(0);
    } else {
        std::cout << std::endl << "------** Received packet's length = " << len_recv << " **------" << std::endl;
    }
    sendAddr = clientAddr;
    len = len_recv;
}