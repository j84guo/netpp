#ifndef TCPCONN_H
#define TCPCONN_H

#include "sockaddr.h"

#include <string>
#include <vector>
#include <cstddef>

/* Wrapper class for OS sockets */
class TCPConn
{
public:
	TCPConn(const std::string &ip, const std::string &port);
	~TCPConn();

	long recv(char *buf, std::size_t num);
	long send(const char *buf, std::size_t num);
	long recvAll(std::vector<char> &buf);
	long sendAll(const char *buf, std::size_t num);

	SockAddr remoteAddr();

private:
	bool connectWithFirst(std::vector<struct addrinfo> &infoVec);
	int sockDes;
	SockAddr remote;
};

#endif
