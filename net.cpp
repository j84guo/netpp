#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

class NetError
{
public:
	NetError(const string &msg): msg(msg) {}
	const string msg;
};

void setLookupHints(struct addrinfo &hints)
{
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

bool lookupHost(const string &host, const string &port, vector<struct addrinfo> &res)
{
	struct addrinfo hints;
	setLookupHints(hints);

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
	int recv(char *buf, int num);
	int send(const char *buf, int num);

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
		throw NetError("Bad host.");

	if (!init(res))
		throw NetError("Could not connect.");
}

TcpConn::~TcpConn()
{
	close(sockDes);
}

int TcpConn::recv(char *buf, int num)
{
	return ::recv(sockDes, buf, num, 0);
}

int TcpConn::send(const char *buf, int num)
{
	return ::send(sockDes, buf, num, 0);
}

bool demo()
{
	TcpConn conn("google.ca", "80");
	string req = "GET / HTTP/1.0\r\n\r\n";
	conn.send(req.c_str(), req.size());

	int num = 0, res;
	vector<char> buf(65535);

	while (num < buf.size()) {
		res = conn.recv(&buf[num], buf.size() - num);
		if (res == -1) {
			perror("recv");
			return false;
		} else if (!res) {
			break;
		}
		num += res;
	}

	cout << string(buf.begin(), buf.begin() + num);
	return true;
}

int main()
{
	return demo() ? 0 : 1;
}
