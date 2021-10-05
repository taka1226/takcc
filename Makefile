CFAGS=-std=c11 -g -static

takcc: takcc.c

test: takcc
		./test.sh

clean: 
		rm -f takcc *.o *~ tmp*

.PHONY: test clean
