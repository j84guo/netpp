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

void lookupHints(struct addrinfo &hints)
{
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

bool lookupHost(const string &host, const string &port, vector<struct addrinfo> &res)
{
	struct addrinfo hints;
	lookupHints(hints);

	struct addrinfo* info;
	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &info))
		return false;
	for (struct addrinfo *ptr = info; ptr; ptr = ptr->ai_next)
		res.emplace_back(*ptr);

	freeaddrinfo(info);
	return true;
}

class TcpConn
{
public:
	TcpConn(const string &ip, const string &port);
	~TcpConn();

	ssize_t recv(char *buf, size_t num);
	ssize_t send(const char *buf, size_t num);
	
	bool recvAll(vector<char> &buf);
	bool sendAll(const char *buf, size_t num);

private:
	bool init(vector<struct addrinfo> &res);
	int sockDes;
};

bool TcpConn::init(vector<struct addrinfo> &res)
{
	for (const auto &info : res) {
		sockDes = socket(info.ai_family, info.ai_socktype, info.ai_protocol);
		if (sockDes == -1)
			continue;
		if (connect(sockDes, info.ai_addr, info.ai_addrlen) == -1) {
			close(sockDes);
			continue;
		}
		return true;
	}
	return false;
}

TcpConn::TcpConn(const string &host, const string &port):
	sockDes(-1)
{
	vector<struct addrinfo> res;
	if (!lookupHost(host, port, res))
		throw NetError("TcpConn", errno);
	if (!init(res))
		throw NetError("TcpConn", errno);
}

TcpConn::~TcpConn()
{
	close(sockDes);
}

ssize_t TcpConn::recv(char *buf, size_t num)
{
	return ::recv(sockDes, buf, num, 0);
}

/* recv until connection close */
bool TcpConn::recvAll(vector<char> &buf)
{
	ssize_t ret;
	size_t num = 0, recv_size = 1024;
	while (1) {
		if (num + recv_size > buf.size())
			buf.resize(buf.size() + recv_size);
		ret = ::recv(sockDes, &buf[num], recv_size, 0);
		if (ret == -1) {
			if (errno == EINTR)
				continue;
			return false;
		} else if (!ret) {
			break;
		}
		num += ret;
	}
	return true;
}

ssize_t TcpConn::send(const char *buf, size_t num)
{
	return ::send(sockDes, buf, num, 0);
}

/* send all bytes from buffer */
bool TcpConn::sendAll(const char *buf, size_t toSend)
{
	size_t num = 0;
	ssize_t ret;
	while (num < toSend) {
		ret = ::send(sockDes, buf, toSend - num, 0);
		if (ret == -1) {
			if (errno == EINTR)
				continue;
			return false;
		}
		num += ret;
	}
	return true;
}

bool demo()
{
	TcpConn conn("google.ca", "80");
	string req = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
	conn.sendAll(req.c_str(), req.size());

	vector<char> buf;
	conn.recvAll(buf);
	cout << string(buf.begin(), buf.end()) << '\n';

	return true;
}

int main()
{
	int status = 0;

	try {
		demo();
	} catch (NetError &e) {
		status = 1;
		cerr << e.what() << '\n';
	}

	return status;
}
