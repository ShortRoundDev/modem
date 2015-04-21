/*Collin Oswalt; 4/20/15
 
   This program is free software: you can redistribute it and/or modify
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

	/*sample size of 74 for 600 baud; half that for 1200*/
#define SAMPLESIZE 74

int main(int argc, char** argv){

  if(argc < 3){
    printf("\nError: Not enough arguments.\n\nUsage: demodulate [input wave file] [output binary file]\n\n");
    exit(1);
  }
  
	/*open transmission file and get file info*/
  SF_INFO transmissionInformation;
  SNDFILE* inputWaveFile = sf_open(argv[1], SFM_READ, &transmissionInformation);
  
  if(inputWaveFile == NULL){
    printf("\nError \"%s\": could not open file!\n\n",argv[1]);
    exit(1);
  }
  
  FILE* outputFile = fopen(argv[2], "wb+");
  	
  if(outputFile == NULL){
    printf("\nError \"%s\": could not create file!\n\n",argv[2]);
    exit(1);
  }
  	/*data buffer for 74 sample audioclip; one bit*/
  short* audioClip = malloc(sizeof(short) * SAMPLESIZE);
  
	/*number of time crossed x axis measures frequency*/
  int numberOfTimesCrossingXAxis = 0;
	/*byte to be written to output binary file*/
  unsigned char writeByte 	 = 0;
  
	/*iterate across the whole file in blocks of SAMPLESIZE * 8 (8 for 8 bits)*/ 
  for(int i = 0; i < transmissionInformation.frames/(SAMPLESIZE*8); i++){
	/*iterate backwards from 7 to add each bit to each place in writeByte*/
    for(int j = 7; j >= 0; j--){      
	/*read 8 74-sample audio clips in the j for-loop*/
      sf_read_short(inputWaveFile, audioClip, SAMPLESIZE);
	/*iterate along each sample in the 74 sample audio-clip*/
      for(int k = 0; k < SAMPLESIZE-1; k++){	
	/*if one sample is < 0, and the other is > 0 (or vice versa), it has crossed the x axis*/
	if((audioClip[k] < 0 && audioClip[k+1] > 0) || (audioClip[k] > 0 && audioClip[k+1] < 0)){	  
	  numberOfTimesCrossingXAxis++;	  
	}
      }
            
      	/*7 times means 2400hz*/
      if(numberOfTimesCrossingXAxis == 7 || numberOfTimesCrossingXAxis == 6 || numberOfTimesCrossingXAxis == 8){
	writeByte += pow(2,j);
      }
	/*3 times means 1200hz*/
      
	/*reset x-axis counter*/
      numberOfTimesCrossingXAxis = 0;
            
                
    }
	/*write to file*/
    putc(writeByte, outputFile);
	/*reset output buffer*/
    writeByte = 0;
  }
	/*commit changes*/
  fclose(outputFile);
}
