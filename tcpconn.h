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

	ssize_t recv(char *buf, std::size_t num);
	ssize_t send(const char *buf, std::size_t num);
	ssize_t recvAll(std::vector<char> &buf);
	ssize_t sendAll(const char *buf, std::size_t num);

	SockAddr remoteAddr();

private:
	bool connectWithFirst(std::vector<struct addrinfo> &infoVec);
	int sockDes;
	SockAddr remote;
};

#endif
