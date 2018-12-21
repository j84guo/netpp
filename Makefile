# Simple installation for Unix-like systems (this can be cleaned up).
install:
	./scripts/install_lib.sh	
	
# Sketchy AF
uninstall:
	./scripts/uninstall_lib.sh	

# Also kind of sketchy
demo:
	g++ -o demo_client.out -Wall -std=c++17 -I src/ examples/demo_client.cpp -lnetpp -lpthread
	g++ -o demo_server.out -Wall -std=c++17 -I src/ examples/demo_server.cpp -lnetpp -lpthread
	g++ -o echo_server.out -Wall -std=c++17 -I src/ examples/echo_server.cpp -lnetpp -lpthread

clean:
	rm -rf *.o *.out

.PHONY: clean install demo
