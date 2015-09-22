#description 

Modem converts binary files into 600baud Kansas city standard audio transmissions. This can then be recorded onto a cassette tape for storage. Demodulate then turns a transmission back into a binary file. A bit of a higher fidelity tape recorder is required to avoid attenuation. An algorithm is used to compensate for loss of samples which I will add a description of later.

#Demo
https://www.youtube.com/watch?v=WrDbnvUK76s

# Dependencies

need libsndfile (available at: http://www.mega-nerd.com/libsndfile/ )

# Compilation

## Linux
simply run make in your terminal

if no makefile is present, or you prefer a different compiler, run:

[cc] modulate.c -o modulate -std=gnu11 -lm -lsndfile && [cc] demodulate.c -o demodulate -std=gnu11 -lm -lsndfile

where [cc] is your compiler (gcc, clang, etc.)

## Windows
Pre-requisites:

* Download C compiler (recommend MinGW)
* Download libsndfile
* Move sndfile.h from /libsndfile/inc -> /MinGW/inc/
* move sndfile-1.lib from /libsndfile/lib -> /MinGW/lib
* move sndfile-1.dll from /libsndFile/bin -> Project folder (should already be included)

open cmd
set path to MinGW\bin folder:

path = [path to MinGW\bin\]

(this should be c:\MinGW\bin\ by default)

change directory to /modem/win32binaries/

run make.bat

##Macintosh

* use brew to install libsndfile
	-> brew install libsndfile
* run make

#Usage

##Modulate

modulate [input file] [output file].wav

input file is any file.

output file is a signed 16 bit pcm 44100 sample rate wav file

##Demodulate

demodulate [input file].wav [output file]

input file is a wave file that follows the standard created by modulate

output file is the name of the binary output file as constructed from input file
