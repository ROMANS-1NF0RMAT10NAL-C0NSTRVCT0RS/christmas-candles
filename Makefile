CFLAGS=-Wall
LDFLAGS=
TARGETS=christmas-candles

all: $(TARGETS)
clean:; rm $(TARGETS)

#%.c: %.pgc; ecpg $<

christmas-candles: christmas-candles.c
	cc $(CFLAGS) $(LDFLAGS) -ldie -lX11 -o $@ $<
