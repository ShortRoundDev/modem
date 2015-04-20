# Dependencies

need libsndfile (available at: http://www.mega-nerd.com/libsndfile/ )

# Compilation

simply run make in your terminal

if no makefile is present, or you prefer a different compiler, run:

[cc] modulate.c -o modulate -std=gnu11 -lm -lsndfile && [cc] demodulate.c -o demodulate -std=gnu11 -lm -lsndfile

where [cc] is your compiler (gcc, clang, etc.)

#Usage

##Modulate

modulate [input file] [output file].wav

input file is any file.

output file is a signed 16 bit pcm 44100 sample rate wav file

##Demodulate

demodulate [input file].wav [output file]

input file is a wave file that follows the standard created by modulate

output file is the name of the binary output file as constructed from input file
