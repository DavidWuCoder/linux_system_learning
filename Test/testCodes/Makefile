.PHONY:all
all: ProcessA ProcessB
ProcessA:ProcessA.cc
	g++ -o ProcessA ProcessA.cc
ProcessB:ProcessB.cc
	g++ -o ProcessB ProcessB.cc
.PHONY:clean
clean:
	rm -rf ProcessA ProcessB