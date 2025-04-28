.PHONY:all
all: tcpclient tcpserver

tcpclient: TcpClient.cc
	g++ -o $@ $^ -std=c++17
tcpserver: TcpServer.cc
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -rf tcpclient tcpserver
