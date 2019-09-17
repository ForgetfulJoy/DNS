#ifndef SOCKETOP_H
#define SOCKETOP_H

#include "socket.h"
#include "packet.h"
#include <cstring>

const std::string SERVER_IP = "114.114.114.114";
const int PORT = 53;
const int MAX_PACKET = 4096;

class socketop {
public:
    int hostSocket;
    socketop();
    ~socketop();
    void sendPacket(Packet &p);
    void recvPacket(unsigned char *buf, sockaddr_in &sendAddr, int &len);
};

#endif // SOCKETOP_H