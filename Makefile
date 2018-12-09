net.out: net.cpp
	g++ -o $@ -Wall -std=c++17 $<

clean:
	rm -rf *.o *.out
