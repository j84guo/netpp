#ifndef TCPCONN_H
#define TCPCONN_H

#include "sockaddr.h"

#include <string>
#include <vector>
#include <cstddef>

/* [NAMESPACE START] net */
namespace net
{

/* A TCP connection class wrapping an OS socket. */
class TCPConn
{
public:
	TCPConn(const std::string &host, const std::string &port);
	TCPConn(int sockDes, const SockAddr &remote);
	TCPConn(const TCPConn &toCopy);
	TCPConn(TCPConn &&toMove);
	~TCPConn();

	TCPConn &operator=(const TCPConn&) = delete;

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

} /* [NAMESPACE END] net */

#endif
