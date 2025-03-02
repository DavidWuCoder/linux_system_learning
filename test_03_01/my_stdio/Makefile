libmyc.so:mystdio.o  mystring.o 
	gcc -shared -o $@ $^
mystdio:mystdio.c
	gcc -fPIC -c $<
mystring:mystring.c
	gcc -fPIC -c $<

.PHONY:output
output:
	mkdir -p lib/include
	mkdir -p lib/mylib
	cp -f *.h lib/include
	cp -f *.so lib/mylib
	tar czf lib.tgz lib

.PHONY:clean
clean:
	rm -rf *.o  libmyc.so lib lib.tgz
