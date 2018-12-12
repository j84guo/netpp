#include "utils.h"
#include "tcpconn.h"
#include "tcpserver.h"

#include <string>
#include <vector>
#include <utility>

#include <cstring>
#include <errno.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

TCPServer::TCPServer(const string & host, const string & port, bool reBind):
	reBind(reBind)
{
	if (port == "")
		throw NetError("TCPServer: Bad port");
	auto res = getAddrInfo(host, port, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP,
			AI_PASSIVE);
	if (res.second)
		throw NetError("TCPServer: " + gaiStrError(res.second));
	if (!bindWithFirst(res.first))
		throw NetError("TCPServer", errno);
	if (!listen())
		throw NetError("TCPServer", errno);
}

TCPServer::~TCPServer()
{
	close(sockDes);
}

SockAddr TCPServer::localAddr()
{
	return local;
}

bool TCPServer::bindWithFirst(vector <struct addrinfo> &infoVec)
{
	for (const auto & info:infoVec) {
		if (!socket(info.ai_family, info.ai_socktype, info.ai_protocol))
			continue;
		if (bind(sockDes, info.ai_addr, info.ai_addrlen) == -1) {
			close(sockDes);
			continue;
		}
		local = SockAddr(*((struct sockaddr_storage *)info.ai_addr),
				 info.ai_addrlen);
		return true;
	}
	return false;
}

bool TCPServer::socket(int family, int type, int protocol)
{
	sockDes = ::socket(family, type, protocol);
	if (sockDes == -1)
		return false;
	
	int val = 1;
	if(setsockopt(sockDes, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val) == -1)
    	return false;

	return true;
}

bool TCPServer::listen()
{
	return ::listen(sockDes, 32) != -1;
}

TCPConn TCPServer::accept()
{
	SockAddr remote;

	int connDes = ::accept(sockDes, remote.saPtr(), remote.saLenPtr());
	if (connDes == -1)
		throw NetError("accept", errno);

	return TCPConn(connDes, remote);
}
