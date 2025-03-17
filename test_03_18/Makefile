.PHONY: all
all: client server

client: Client.cc
	g++ -o $@ $^

server: Server.cc
	g++ -o $@ $^

.PHONY: clean
clean:
	rm -rf client server