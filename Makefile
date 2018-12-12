demo.out:
	g++ -o $@ -Wall -std=c++17 *.cpp -lpthread

clean:
	rm -rf *.o *.out

.PHONY: clean demo.out
