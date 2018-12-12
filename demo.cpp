#include "net.h"

#include <thread>
#include <string>
#include <vector>
#include <cstddef>
#include <iostream>

using namespace std;

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
	vector<char> buf;
	long numRecvd = conn.recvAll(buf);
	cout << string(buf.begin(), buf.begin() + numRecvd);
}

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
	TCPServer server("0.0.0.0", "8000");
	cout << "Started server: " << server.localAddr() << '\n';

	while (1) {
		/*
		 * The server accepts a new connection and returns to us a TCPConn
		 * object to communicate with the client
		 */
		TCPConn conn = server.accept();

		/*
		 * We use a simple concurreny model: each connection gets handled by
         * a new thread
         */
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
