#include <netpp/net.h>

#include <thread>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

using net::TCPConn;
using net::TCPListener;
using net::NetError;

using std::cerr;
using std::cout;
using std::move;
using std::string;
using std::thread;
using std::vector;

void handleConn(TCPConn conn)
{
	try {
		vector<char> buf(4096);
		while (1) {	
			long num = conn.recv(&buf[0], buf.size());
			if (!num)
				break;
			conn.sendAll(&buf[0], num);
		}
		cout << "Client disconnected: " << conn.remoteAddr() << '\n';
	} catch (NetError e) {
		cerr << e.what() << '\n';
	}
}

void echoServer()
{
	TCPListener server("127.0.0.1", "8000");
	cout << "Started server: " << server.localAddr() << '\n';
	while (1) {
		TCPConn conn = server.accept();
		cout << "Accepting client: " << conn.remoteAddr() << '\n';
		thread t(handleConn, move(conn));
		t.detach();
	}
}

int main(int argc, char *argv[])
{
	try {
		echoServer();
	} catch (NetError &e) {
		cerr << e.what() << '\n';
		return 1;
	}
	return 0;
}
