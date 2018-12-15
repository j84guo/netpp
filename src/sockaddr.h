#ifndef SOCKADDR_H
#define SOCKADDR_H

#include <string>
#include <iostream>
#include <sys/socket.h>

namespace net
{

class SockAddr
{
public:
	SockAddr();
	SockAddr(struct sockaddr_storage &from, socklen_t saLen);

	std::string getIP() const;
	int getFamily() const;
	int getPort() const;

	struct sockaddr *saPtr();
	socklen_t *saLenPtr();
	std::string toString() const;

private:
	struct sockaddr_storage sa;
	socklen_t saLen;
};

std::ostream& operator<<(std::ostream &out, const SockAddr &addr);

} /* namespace net */

#endif
