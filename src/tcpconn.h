#ifndef TCPCONN_H
#define TCPCONN_H

#include "sockaddr.h"

#include <string>
#include <vector>
#include <cstddef>

namespace net
{

/* A TCP connection class wrapping an OS socket. */
class TCPConn
{
public:
	/* constructors for client and server side use, plus efficient copy/move */
	TCPConn(const std::string &host, const std::string &port);
	TCPConn(int sockDes, const SockAddr &remote);
	TCPConn(const TCPConn &toCopy);
	TCPConn(TCPConn &&toMove);
	~TCPConn();

	/* No assignment should ever be necessary */
	TCPConn &operator=(const TCPConn&) = delete;

	/* I/O operatrions throw NetError on failure */
	long recv(char *buf, std::size_t num);
	long send(const char *buf, std::size_t num);
	long recvAll(std::vector<char> &buf);
	std::vector<char> recvAll();
	long sendAll(const char *buf, std::size_t num);

	/* Get a copy of the remote socket's address */
	SockAddr remoteAddr();

private:
	bool connectWithFirst(std::vector<struct addrinfo> &infoVec);
	int sockDes;
	SockAddr remote;
};

} /* namespace net */

#endif
