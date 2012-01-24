CFLAGS=-Wall
TARGETS=adjust-yahoo-ohlc christmas-candles

all: $(TARGETS)
clean:; rm $(TARGETS)

adjust-yahoo-ohlc: adjust-yahoo-ohlc.c
	cc $(CFLAGS) -o $@ $<

christmas-candles: christmas-candles.c
	cc $(CFLAGS) -lX11 -ldie -lm -o $@ $<
