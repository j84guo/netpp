#include <netpp/net.h>

#include <string>
#include <vector>
#include <iostream>

using net::TCPConn;
using net::NetError;

using std::cerr;
using std::cout;
using std::string;
using std::vector;

void demoClient(const string &host, const string &port)
{
	/*
	 * HTTP/1.1 has the Connection: close header, which instructs the server to
	 * close() the TCP connection after sending the response. This makes it
	 * easy for us to demo this socket library without having to parse the
	 * HTTP response. We simply read until recv() returns 0.
	 */ 
	TCPConn conn(host, port);

	/*
	 * For convenience, TCPConn::sendAll retries send on partial writes (and
	 * on interruption by an OS signal) until all bytes are sent.
	 */
	string req = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
	conn.sendAll(req.c_str(), req.size());

	/* Similarly, we can read all bytes sent by the client in one call */
	vector<char> buf = conn.recvAll();
	cout << string(buf.begin(), buf.end());
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " <domainName> <portNumber>\n";
		return 1;
	}

	try {
		demoClient(argv[1], argv[2]);
	} catch (NetError &e) {
		cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}
