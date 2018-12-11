#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

#include <cstdio>
#include <cstddef>
#include <cstring>
#include <errno.h>
#include <unistd.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

string strError(int err)
{
	char buf[128] = {0}, *ptr = nullptr;
#if defined(__GLIBC__) && defined(_GNU_SOURCE)                             
	ptr = strerror_r(err, buf, sizeof(buf) - 1);                  
#else
	strerror_r(err, buf, sizeof(buf) - 1);      
	ptr = buf;              
#endif
	return ptr;
}

class NetError : public runtime_error
{
public:
	NetError(const string &msg):
		runtime_error(msg) { }
	NetError(int err):
		runtime_error(strError(err)) { }
	NetError(const string &prefix, int err):
		runtime_error(prefix + ": " + strError(err)) { }
};

struct addrinfo lookupHints()
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	return hints;
}

/* throws NetError on getaddrinfo failure */
vector<struct addrinfo> lookupHost(const string &host, const string &port)
{
	struct addrinfo hints = lookupHints();
	vector<struct addrinfo> infoVec;

	struct addrinfo* info;
	int ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &info);
	if (ret)
		throw NetError(string("lookupHost: ") + gai_strerror(ret));
	for (struct addrinfo *ptr = info; ptr; ptr = ptr->ai_next)
		infoVec.push_back(*ptr);

	freeaddrinfo(info);
	return infoVec;
}

class TCPConn
{
public:
	TCPConn(const string &ip, const string &port);
	~TCPConn();

	ssize_t recv(char *buf, size_t num);
	ssize_t send(const char *buf, size_t num);	
	ssize_t recvAll(vector<char> &buf);
	ssize_t sendAll(const char *buf, size_t num);

private:
	void connectWithFirst(vector<struct addrinfo> &infoVec);
	int sockDes;
};

/* throws NetError if all connection attempts fail */
void TCPConn::connectWithFirst(vector<struct addrinfo> &infoVec)
{
	for (const auto &info : infoVec) {
		sockDes = socket(info.ai_family, info.ai_socktype, info.ai_protocol);
		if (sockDes == -1)
			continue;
		if (connect(sockDes, info.ai_addr, info.ai_addrlen) == -1) {
			close(sockDes);
			continue;
		}
		return;
	}
	throw NetError("initConn", errno);
}

TCPConn::TCPConn(const string &host, const string &port):
	sockDes(-1)
{
	vector<struct addrinfo> infoVec = lookupHost(host, port);
	connectWithFirst(infoVec);
}

TCPConn::~TCPConn()
{
	close(sockDes);
}

ssize_t TCPConn::recv(char *buf, size_t num)
{
	return ::recv(sockDes, buf, num, 0);
}

/* recv until connection close */
ssize_t TCPConn::recvAll(vector<char> &buf)
{
	size_t num = 0, RECV_SIZE = 4096;
	while (1) {
		if (num + RECV_SIZE > buf.size())
			buf.resize(buf.size() + RECV_SIZE);
		ssize_t ret = ::recv(sockDes, &buf[num], RECV_SIZE, 0);
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

ssize_t TCPConn::send(const char *buf, size_t num)
{
	return ::send(sockDes, buf, num, 0);
}

/* send all bytes from buffer */
ssize_t TCPConn::sendAll(const char *buf, size_t toSend)
{
	size_t num = 0;
	while (num < toSend) {
		ssize_t ret = ::send(sockDes, buf, toSend - num, 0);
		if (ret == -1) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		num += ret;
	}
	return num;
}

void demo(const string &host, const string &port)
{
	TCPConn conn(host, port);
	string req = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
	conn.sendAll(req.c_str(), req.size());

	vector<char> buf;
	ssize_t num = conn.recvAll(buf);
	cout << string(buf.begin(), buf.begin() + num);
}

/**
 * Todo:
 * Use a reader/writer interface instead of recv/send, that way buffered
 * wrappers/scanners can easily be made?
 * Also add socket options like non-blocking, timeout, etc.
 * How to select/poll on multiple TCPConns
 * Add UDPConn
 * Throw exception from within lookupHost and init...
 */
int main(int argc, char *argv[])
{
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " <host> <port>\n";
		return 1;
	}

	try {
		demo(argv[1], argv[2]);
	} catch (NetError &e) {
		cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}
