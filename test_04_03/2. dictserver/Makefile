.PHONY:all
all: udpclient udpserver

udpclient:UdpClient.cc
	g++ -o $@ $^ -std=c++17 -static
udpserver:UdpServer.cc
	g++ -o $@ $^ -std=c++17
.PHONY:
clean:
	rm -rf udpclient udpserver

