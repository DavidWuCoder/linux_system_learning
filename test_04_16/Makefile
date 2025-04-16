.PHONY:all
all: tcpserver

tcpserver:main.cc
	g++ -o $@ $^ -std=c++17 -lpthread
.PHONY:
clean:
	rm -rf tcpserver

