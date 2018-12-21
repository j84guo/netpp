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
	/*
	 * Each TCPConn has a SockAddr object, which holds the IP and port
	 * information of the remote client. SockAddr is printable, so we can just
	 * cout.
	 */
	cout << "Handling client: " << conn.remoteAddr() << '\n';
	try {
		vector<char> buf = conn.recvAll();
		cout << string(buf.begin(), buf.end());
	} catch (NetError &e) {
		cerr << e.what() << '\n';
	}
}

void demoServer()
{
	/*
 	 * The TCPListener object acquires a socket descriptor, binds to a local IP
 	 * address and port (passed to it in the constructor) and listens for
 	 * connection requests.
 	 */ 
	TCPListener server("127.0.0.1", "8000");
	cout << "Started server: " << server.localAddr() << '\n';

	while (1) {
		/*
		 * The server accepts a new connection and returns to us a TCPConn
		 * object to communicate with the client. Note TCPListener::accept blocks
		 * indefinitely until a connection request is recieved, as expected.
		 */
		TCPConn conn = server.accept();

		/*
		 * We use a simple concurrency model: each connection gets handled by
		 * a new thread.
		 *
		 * The std::thread constructor accepts a function pointer (handleConn)
		 * and a variable list of arguments to call the function pointer with.
		 * It then asks the OS to create a separate execution context (a thread
		 * of execution) in which the function pointer is called.
		 *
		 * Detaching a thread is a common concept across languages. It allows
		 * the newly created thread to run independently of the main thread.
		 * The implications of detaching a thread vs its analogue, joining, are
		 * described in: man pthread_detach.
		 *
		 * Note the usage of std::move is a little bit advanced, simply passing
		 * conn (resulting in a cheap copy constructor call) is also ok.
		 */
		thread t(handleConn, move(conn));
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
