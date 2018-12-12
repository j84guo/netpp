#include "tcpconn.h"
#include "utils.h"

#include <string>
#include <vector>
#include <utility>

#include <cstddef>
#include <cstring>
#include <errno.h>
#include <unistd.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

TCPConn::TCPConn(int sockDes, const SockAddr &remote):
	sockDes(sockDes),
	remote(remote)
{ }

TCPConn::TCPConn(const string &host, const string &port):
	sockDes(-1)
{
	if (port == "")
		throw NetError("TcpConn: Bad port");
	auto res = getAddrInfo(host, port, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	if (res.second)
		throw NetError(string("TcpConn: ") + gaiStrError(res.second));
	if (!connectWithFirst(res.first))
		throw NetError("TcpConn", errno);
}

TCPConn::~TCPConn()
{
	close(sockDes);
}

SockAddr TCPConn::remoteAddr()
{
	return remote;
}

bool TCPConn::connectWithFirst(vector<struct addrinfo> &infoVec)
{
	for (const auto &info : infoVec) {
		sockDes = socket(info.ai_family, info.ai_socktype, info.ai_protocol);
		if (sockDes == -1)
			continue;
		if (connect(sockDes, info.ai_addr, info.ai_addrlen) == -1) {
			close(sockDes);
			continue;
		}
		remote = SockAddr(*((struct sockaddr_storage *) info.ai_addr),
				info.ai_addrlen);
		return true;
	}
	return false;
}

long TCPConn::recv(char *buf, size_t num)
{
	return ::recv(sockDes, buf, num, 0);
}

long TCPConn::recvAll(vector<char> &buf)
{
	size_t num = 0, RECV_SIZE = 4096;
	while (1) {
		if (num + RECV_SIZE > buf.size())
			buf.resize(buf.size() + RECV_SIZE);
		long ret = ::recv(sockDes, &buf[num], RECV_SIZE, 0);
		if (ret == -1) {
			if (errno == EINTR)
				continue;
			return -1;
		} else if (!ret) {
			break;
		}
		num += ret;
	}
	return num;
}

long TCPConn::send(const char *buf, size_t num)
{
	return ::send(sockDes, buf, num, 0);
}

long TCPConn::sendAll(const char *buf, size_t toSend)
{
	size_t num = 0;
	while (num < toSend) {
		long ret = ::send(sockDes, buf, toSend - num, 0);
		if (ret == -1) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		num += ret;
	}
	return num;
}
