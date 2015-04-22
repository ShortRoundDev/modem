CFLAGS=-std=gnu11
LDFLAGS=-lm -lsndfile

all: modulate demodulate
modulate: modulate.c
demodulate: demodulate.c
