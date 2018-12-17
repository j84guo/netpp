#include "utils.h"
#include "tcpconn.h"
#include "tcplistener.h"

#include <string>
#include <vector>
#include <utility>

#include <cstring>
#include <errno.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

using std::string;
using std::vector;

net::TCPListener::TCPListener(const string & host, const string & port, bool reBind):
	reBind(reBind)
{
	if (port == "")
		throw NetError("TCPListener: Bad port");
	auto res = getAddrInfo(host, port, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP,
			AI_PASSIVE);
	if (res.second)
		throw NetError("TCPListener: " + gaiStrError(res.second));
	if (!bindWithFirst(res.first))
		throw NetError("TCPListener", errno);
	if (!listen())
		throw NetError("TCPListener", errno);
}

net::TCPListener::~TCPListener()
{
	close(sockDes);
}

net::SockAddr net::TCPListener::localAddr()
{
	return local;
}

bool net::TCPListener::bindWithFirst(vector <struct addrinfo> &infoVec)
{
	for (const auto & info:infoVec) {
		if (!socket(info.ai_family, info.ai_socktype, info.ai_protocol))
			continue;
		if (::bind(sockDes, info.ai_addr, info.ai_addrlen) == -1) {
			close(sockDes);
			continue;
		}
		local = SockAddr(*((struct sockaddr_storage *)info.ai_addr),
				 info.ai_addrlen);
		return true;
	}
	return false;
}

bool net::TCPListener::socket(int family, int type, int protocol)
{
	sockDes = ::socket(family, type, protocol);
	if (sockDes == -1)
		return false;

	int val = 1;
	if(reBind && setsockopt(sockDes, SOL_SOCKET, SO_REUSEADDR, &val,
			sizeof val) == -1)
		return false;

	return true;
}

bool net::TCPListener::listen()
{
	return ::listen(sockDes, 32) != -1;
}

net::TCPConn net::TCPListener::accept()
{
	SockAddr remote;

	int connDes = ::accept(sockDes, remote.saPtr(), remote.saLenPtr());
	if (connDes == -1)
		throw NetError("TCPListener::accept", errno);

	return TCPConn(connDes, remote);
}
