#include "utils.h"
#include "tcpconn.h"

#include <string>
#include <vector>
#include <utility>

#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

using std::pair;
using std::size_t;
using std::string;
using std::vector;

/*
 * For use by TCPServer::accept(). We still verify that the file descrptor is
 * valid in order to prevent objects existing in an invalid state.
 */
TCPConn::TCPConn(int sockDes, const SockAddr &remote):
	sockDes(sockDes),
	remote(remote)
{
	if (fcntl(sockDes, F_GETFD) == -1)
		throw NetError("TCPConn", errno);
}

/*
 * DNS lookup, if necessary, then connect.
 */
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

/*
 * By definition "copy" should not involve stealing or sharing file
 * descriptors. Therefore, we dup() the toCopy object into the new one.
 */
TCPConn::TCPConn(const TCPConn &toCopy):
	remote(toCopy.remote)
{
	sockDes = dup(toCopy.sockDes);
	if (sockDes == -1)
		throw NetError("TCPConn", errno);
}

/*
 * Verify that the file descriptor we "steal" from the old object is valid,
 * throw NetError if not.
 */
TCPConn::TCPConn(TCPConn &&toMove):
	sockDes(toMove.sockDes),
	remote(toMove.remote)
{
	if (fcntl(sockDes, F_GETFD) == -1)
		throw NetError("TCPConn", errno);
	toMove.sockDes = -1;
}

/*
 * If sockDes is valid, close it on destruct. Our move constructor sets sockDes
 * to -1 when it "steals" the file descriptor of an old object, hence the old
 * object needs to verify that sockDes >= 0 before closing. If we didn't do
 * this check, we would close a file descriptor that's still in use by a
 * moved object! Sad.
 */
TCPConn::~TCPConn()
{
	if (sockDes >= 0)
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
