#include <netpp/net.h>

#include <thread>
#include <string>
#include <vector>
#include <iostream>

using net::TCPConn;
using net::TCPServer;
using net::NetError;

using std::cerr;
using std::cout;
using std::string;
using std::thread;
using std::vector;

void handleConn(TCPConn conn)
{
	/*
	 * Each TCPConn has a SockAddr object, which holds the IP and port
	 * information of the remote client. SockAddr is printable, so we can just
	 * cout.
	 */
	cout << "Handling client: " << conn.remoteAddr() << '\n';
	vector<char> buf;
	long numRecvd = conn.recvAll(buf);
	cout << string(buf.begin(), buf.begin() + numRecvd);
}

void demoServer()
{
	/*
 	 * The TCPServer object acquires a socket descrptor, binds to a local IP
 	 * address and port (passed to it in the constructor) and listens for
 	 * connection requests.
 	 */ 
	TCPServer server("127.0.0.1", "8000");
	cout << "Started server: " << server.localAddr() << '\n';

	while (1) {
		/*
		 * The server accepts a new connection and returns to us a TCPConn
		 * object to communicate with the client. Note TCPServer::accept blocks
		 * indefinitely until a connection request is recieved, as expected.
		 */
		TCPConn conn = server.accept();

		/*
		 * We use a simple concurreny model: each connection gets handled by
		 * a new thread.
		 */
		thread t(handleConn, conn);
		t.detach();
	}
}

int main(int argc, char *argv[])
{
	try {
		demoServer();
	} catch (NetError &e) {
		cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}
