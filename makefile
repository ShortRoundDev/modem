all:
	gcc modulate.c -o modulate -std=gnu11 -lm -lsndfile && gcc demodulate.c -o demodulate -std=gnu11 -lm -lsndfile
