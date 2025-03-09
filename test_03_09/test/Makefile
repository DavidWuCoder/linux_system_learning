.PHONY:all
all:testSig mykill
testSig:testSig.cc
	g++ -o $@ $^
mykill:mykill.cc
	g++ -o $@ $^
.PHONY:
clean:
	rm -rf testSig mykill