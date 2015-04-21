/*Collin Oswalt - 4/20/15
* this program modulates a binary file into 
* 16 bit 44100hz pcm sound wave. 1200 for 0,
* 2400 for 1

takes argv[1] for input and argv[2] for output*/

/*This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <math.h>

#define SAMPLESIZE 	74
#define PI		3.141592

	/*returns -1 if <0, 1 if >= 0*/
signed short returnParity(double input);
	/*returns character arrayrepresentation of the number in binary*/
unsigned char* charToBinary(unsigned char input);

int main(int argc, char** argv){	

	if(argc < 3){
		printf("\nError: Not enough arguments.\n\nUsage: modulate [input file] [output file].wav\n\n");
		exit(1);
	}

		/*File to be modulated into PCM*/
	FILE* fileInput;

			/*argv[1] is name of input file. go to end of file,
			* get byte #, return as size of file in bytes*/
		fileInput = fopen(argv[1], "rb");
		if(fileInput == NULL){
			printf("\nError \"%s\": No such file found!\n\n",argv[1]);
			exit(1);
		}	
					fseek(fileInput, 0, SEEK_END);	
	int fileSize = 	ftell(fileInput);
				   rewind(fileInput);

		/*WAV file, signed 16-bit pcm, 44100 (cd quality) sample rate, 1 channel*/
	SF_INFO transInformation;
		transInformation.channels	= 1;
		transInformation.samplerate	= 44100;
		transInformation.format		= SF_FORMAT_WAV | SF_FORMAT_PCM_16;

		/*argv[2] is output file name*/
	SNDFILE* transmissionFile = sf_open(argv[2], SFM_WRITE, &transInformation);

	if(transmissionFile == NULL){
		printf("\nError \"%s\": could not create file!\n\n",argv[2]);
		exit(1);
	}

		/*prebuffered 1200hz square wave*/
	short square1200[SAMPLESIZE];
		/*prebuffered 2400hz square wave*/
	short square2400 [SAMPLESIZE];

		/*dont know why, but data got all wonky when I put both the assignments in the same for loop*/
	for(int i = 0; i < SAMPLESIZE; i++){				
		square2400[i] = returnParity(sin(((i * (2 * PI))/44100) * 2400)) * ((pow(2,15)-1) * 0.8);
	}
	for(int i = 0; i < SAMPLESIZE; i++){
		square1200[i] = returnParity(sin(((i * (2 * PI))/44100) * 1200)) * ((pow(2,15)-1) * 0.8);				
	}

		/*individual byte; read and write one byte at a time to save memory usage*/
	char 	fileDataBuffer;
		/*fileDataBuffer expanded into binary*/
	char* 	fileDataBufferBinary;	
	for(int i = 0; i < fileSize; i++){		

			/*get progress*/
		if(i == (int)(fileSize/4)){
			printf("25%%\n");
		}
		if(i == (int)(fileSize/2)){
			printf("50%%\n");
		}
		if(i == (int)(3 * fileSize/4)){
			printf("75%%\n");		
		}

			/*get one byte*/
		fread(&fileDataBuffer, 1, 1, fileInput);		
			/*convert to binary*/
		fileDataBufferBinary = charToBinary(fileDataBuffer);
			/*iterate along binary data buffer, char by char*/
		for(int j = 0; j < 8; j++){			
				/*write 2400hz for 1*/
			if(fileDataBufferBinary[j] == '1'){
					/*for some reason data gets screwed up if short
					* is written by more than one sample at a time*/
				for(int k = 0; k < SAMPLESIZE; k++){					
					sf_write_short(transmissionFile, &square2400[k], 1);
				}
			}
				/*write 1200hz for 0*/
			else if(fileDataBufferBinary[j] == '0'){						
				for(int k = 0; k < SAMPLESIZE; k++){							
					sf_write_short(transmissionFile, &square1200[k], 1);
				}
			}
		}

		free(fileDataBufferBinary);
	}	

		/*commit changes*/
	sf_write_sync(transmissionFile);
		/*save*/
	sf_close(transmissionFile);

}

unsigned char* charToBinary(unsigned char input){
	unsigned char *binaryString = malloc(8);

		/*initialize string with 0 so if the division algorithm ends
		* before 8 bits (input < 128), I can be sure the string will
		* be represented accurately*/
	for(int i = 0; i < 8; i++){
		binaryString[i] = '0';		
	}
	int numerator = input;	

		/*division algorithm*/
	for(int i = 7; i >= 0; i--){
		binaryString[i] = (numerator % 2)+48;		
		numerator /= 2;
	}

	return binaryString;
}

signed short returnParity(double input){

	if(input < 0){		
		return -1;
	}
		/*0 is inclusive to 1 so that sin(0) returns 1,
		* and each wave begins positive and ends negative*/
	else{		
		return 1;
	}
}
