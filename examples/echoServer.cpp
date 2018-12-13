#include <netpp/net.h>

#include <thread>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

using net::TCPConn;
using net::TCPServer;
using net::NetError;

using std::cerr;
using std::cout;
using std::move;
using std::string;
using std::thread;
using std::vector;

void handleConn(TCPConn conn)
{
	vector<char> buf(4096);
	while (1) {	
		long num = conn.recv(&buf[0], 4096);
		if (!num)
			break;
		conn.sendAll(&buf[0], num);
	}
	cout << "Client " << conn.remoteAddr() << " disconnected\n"; 
}

void echoServer()
{
	TCPServer server("127.0.0.1", "8000");
	cout << "Started server: " << server.localAddr() << '\n';

	while (1) {
		TCPConn conn = server.accept();
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
