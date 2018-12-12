#include "sockaddr.h"

#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

SockAddr::SockAddr()
{
	saLen = sizeof(struct sockaddr_storage);
	memset(&sa, 0, saLen);
	sa.ss_family = AF_UNSPEC;
}

SockAddr::SockAddr(struct sockaddr_storage &sa, socklen_t saLen):
	sa(sa),
	saLen(saLen)
{ }

int SockAddr::getFamily()
{
	return sa.ss_family;
}

int SockAddr::getPort()
{
	switch (getFamily()) {
	case AF_INET:
		return ntohs(((struct sockaddr_in *) &sa)->sin_port);
	case AF_INET6:
		return ntohs(((struct sockaddr_in6 *) &sa)->sin6_port);
	default:
		return -1;
	}
}

string SockAddr::getIP()
{
	int fam = getFamily();
	char buf[INET6_ADDRSTRLEN] = {0};

	void *addr;
	switch (fam) {
	case AF_INET:
		addr = (void *) &((struct sockaddr_in *) &sa)->sin_addr;
		break;
	case AF_INET6:
		addr = (void *) &((struct sockaddr_in6 *) &sa)->sin6_addr;
		break;
	default:
		return buf;
	}

	inet_ntop(fam, addr, buf, sizeof(buf));
	return buf;
}

struct sockaddr *SockAddr::saPtr()
{
	return (struct sockaddr *) &sa;
}

socklen_t *SockAddr::saLenPtr()
{
	return &saLen;
}
