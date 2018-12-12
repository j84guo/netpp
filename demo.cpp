#include "net.h"

#include <string>
#include <vector>
#include <cstddef>
#include <iostream>

using namespace std;

void demo(const string &host, const string &port)
{
	TCPConn conn(host, port);
	string req = "GET / HTTP/1.1\r\nConnection: close\r\n\r\n";
	conn.sendAll(req.c_str(), req.size());

	vector<char> buf;
	long num = conn.recvAll(buf);
	cout << string(buf.begin(), buf.begin() + num);

	SockAddr sa = conn.remoteAddr();
	cout << sa.getFamily() << ' ' << sa.getPort() << ' ' << sa.getIP() << '\n';
}

/**
 * Todo:
 * - Make SockAddr::saLen accessible for accept()
 * - Constructor accepting an fd + SockAddr for server accept()
 * - Use a reader/writer interface instead of recv/send, that way buffered
 *   wrappers/scanners can easily be made?
 * - Socket options like non-blocking, timeout
 * - How to select/poll on multiple TCPConns
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
