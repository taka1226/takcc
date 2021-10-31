CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

takcc: $(OBJS)
	$(CC) -o takcc $(OBJS) $(LDFLAGS)

$(OBJS): takcc.h

test: takcc
	./test.sh

clean:
	rm -f takcc *.o *~ tmp*

.PHONY: test clean
