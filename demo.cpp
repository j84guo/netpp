#include "net.h"

#include <thread>
#include <string>
#include <vector>
#include <cstddef>
#include <iostream>

using namespace std;

void demoClient(const string &host, const string &port)
{
	TCPConn conn(host, port);
	string req = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
	conn.sendAll(req.c_str(), req.size());

	vector<char> buf;
	long num = conn.recvAll(buf);
	cout << string(buf.begin(), buf.begin() + num);
}

void handleConn(TCPConn conn)
{
	cout << "Handling client: " << conn.remoteAddr() << '\n';
	vector<char> buf;
	long num = conn.recvAll(buf);
	cout << string(buf.begin(), buf.begin() + num);
}

void demoServer()
{
	TCPServer server("0.0.0.0", "8000");
	cout << "Started server: " << server.localAddr() << '\n';

	while (1) {
		TCPConn conn = server.accept();
		thread t(handleConn, conn);
		t.detach();
	}
}

/**
 * Todo:
 * - Figure out which objects can/cannot be copied/moved
 * - Re-think file descriptor verification
 *
 * - Use a reader/writer interface instead of recv/send, that way buffered
 *   wrappers/scanners can easily be made?
 * - Socket options like non-blocking, timeout
 * - How to select/poll on multiple TCPConns
 */
int main(int argc, char *argv[])
{
	try {
		if (argc == 3)
			demoClient(argv[1], argv[2]);
		else
			demoServer();
	} catch (NetError &e) {
		cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}
