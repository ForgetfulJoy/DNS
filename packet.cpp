#include "packet.h"
#include "socket.h"
#include <iostream>
#include <algorithm>

Packet::Packet() {

}

Packet::Packet(const sockaddr_in send_addr) {
	this->send_addr = send_addr;
	this->ans.clear();
	this->add.clear();
	this->authority.clear();
}

void Packet::getHeader(unsigned char *&header_p) {
    unsigned short *buf = (unsigned short *)header_p;
    header.id = ntohs(*(buf));
    unsigned short header_buf = ntohs(*(buf + 1));
    header.qr     = (header_buf & 0x8000) >> 15;
	header.opcode = (header_buf & 0x7800) >> 11;
	header.aa     = (header_buf & 0x0400) >> 10;
	header.tc     = (header_buf & 0x0200) >>  9;
	header.rd     = (header_buf & 0x0100) >>  8;
	header.ra     = (header_buf & 0x0080) >>  7;
	header.zero   = (header_buf & 0x0070) >>  4;
	header.rcode  = (header_buf & 0x000F);

	header.qdcount = ntohs(*(buf + 2));
	header.ancount = ntohs(*(buf + 3));
	header.nscount = ntohs(*(buf + 4));
	header.arcount = ntohs(*(buf + 5));

    header_p += 12;
}

void Packet::getQuery(unsigned char *&buf) {	//将收到的Query转化成string，加入到query的vector中
	for (int c = 0; c < header.qdcount; c++) {
		std::string namebuf = "";
		Query q;
		char *tmp = (char *)buf;
		while (*tmp != 0) {
			namebuf.append(1, *tmp);
			tmp++;
		}
		namebuf.append(1, '\0');
		q.qname = namebuf;
		buf = (unsigned char *)tmp;
		buf++;
		q.qtype = ntohs(*(unsigned short *)buf);
		buf += 2;
		q.qclass = ntohs(*(unsigned short *)buf);
		buf += 2;
		query.push_back(q);
	}
}

void Packet::getDomainname(std::string &buf, std::string name) {
	const char *namebuf = name.c_str();
	while (*namebuf != '\0') {
		int len = *namebuf;
		namebuf++;
		buf.append(namebuf, len);
		namebuf += len;
		if(*namebuf != '\0') {
			buf.append(1, '.');
		}
	}
	transform(buf.begin(), buf.end(), buf.begin(), ::tolower);
}

void Packet::getResource(unsigned char *&buf, std::vector<Resource> &buf_rsc, int rcount) {
	for (int i = 0; i < rcount; i++) {
		std::string namebuf = "";
		Resource r;
		if (*buf == 0xc0) {
			buf += 2;
			r.rname = query.begin()->qname;
		} else {
			char *tmp = (char *)buf;
			while (*tmp != 0) {
				namebuf.append(1, *tmp);
				tmp++;
			}
			namebuf.append(1, '\0');
			r.rname = namebuf;
			buf = (unsigned char *)tmp;
			buf++;
		}
		r.rtype = ntohs(*(unsigned short *)buf);
		buf += 2;
		r.rclass = ntohs(*(unsigned short *)buf);
		buf += 2;
		r.rtime = ntohl(*(unsigned int *)buf);
		buf += 4;
		r.rlength = ntohs(*(unsigned short *)buf);
		buf += 2;
		r.rdata.assign(buf, buf + r.rlength);
		buf += r.rlength;
		buf_rsc.push_back(r);
	}
}

void Packet::byteTobuf2(unsigned short v, unsigned char *&buf, int &bufsize) {
	unsigned short *tmp = (unsigned short *)buf;
	*tmp = (unsigned short)(htons(v));
	bufsize += sizeof(unsigned short);
	buf += sizeof(unsigned short);
	if (bufsize > 4096) {
		std::cout << std::endl << "------** The buffer if full **------" << std::endl;
		exit(0);
	}
}

void Packet::byteTobuf4(unsigned int v, unsigned char *&buf, int &bufsize) {
	unsigned int *tmp = (unsigned int *)buf;
	*tmp = (unsigned int)(htonl(v));
	bufsize += sizeof(unsigned int);
	buf += sizeof(unsigned int);
	if (bufsize > 4096) {
		std::cout << std::endl << "------** The buffer is full **------" << std::endl;
	}
}

void Packet::setHeader(unsigned short &headerbuf) {
	headerbuf = headerbuf | (header.qr << 15);
	headerbuf = headerbuf | (header.opcode << 11);
	headerbuf = headerbuf | (header.aa << 10);
	headerbuf = headerbuf | (header.tc << 9);
	headerbuf = headerbuf | (header.rd << 8);
	headerbuf = headerbuf | (header.ra << 7);
	headerbuf = headerbuf | (header.zero << 4);
	headerbuf = headerbuf | (header.rcode);
}

void Packet::queryTobuf(unsigned char *&buf, int &bufsize) {
	std::vector<Query>::iterator i = query.begin();
	for (; i != query.end(); i++) {
		int len = i->qname.size();
		const char *name_tmp = i->qname.c_str();
		memcpy(buf, name_tmp, len);
		buf = buf + len;
		bufsize = bufsize + len;
		byteTobuf2(i->qtype, buf, bufsize);
		byteTobuf2(i->qclass, buf, bufsize);
	}
}

void Packet::resourceTobuf(std::vector<Resource> r, unsigned char *&buf, int &bufsize) {
	if (r.size() == 0) {
		return;
	}
	std::vector<Resource>::iterator i = r.begin();
	for(; i != r.end(); i++) {
		int len = i->rname.size();
		const char *name_tmp = i->rname.c_str();
		memcpy(buf, name_tmp, len);
		buf += len;
		bufsize += len;
		byteTobuf2(i->rtype, buf, bufsize);
		byteTobuf2(i->rclass, buf, bufsize);
		byteTobuf4(i->rtime, buf, bufsize);
		byteTobuf2(i->rlength, buf, bufsize);
		int s = i->rdata.size();
		std::vector<unsigned char>::iterator index = i->rdata.begin();
		for(; index != i->rdata.end(); index++) {
			unsigned char d;
			d = *index;
			memcpy(buf, &d, sizeof(unsigned char));
			buf += sizeof(unsigned char);
		}
		bufsize += s;
	}
}