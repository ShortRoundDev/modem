#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <math.h>

#define SAMPLESIZE 74

int main(int argc, char** argv){

  if(argc < 3){
    printf("\nError: Not enough arguments.\n\nUsage: demodulate [input wave file] [output binary file]\n\n");
    exit(1);
  }
  
  SF_INFO transmissionInformation;
  SNDFILE* inputWaveFile = sf_open(argv[1], SFM_READ, &transmissionInformation);
  
  if(inputWaveFile == NULL){
    printf("\nError \""); printf("%s",argv[1]); printf("\": could not open file!\n\n");
    exit(1);
  }
  
  FILE* outputFile = fopen(argv[2], "wb+");
  
  if(outputFile == NULL){
    printf("\nError \""); printf("%s", argv[2]); printf("\": could not create file!\n\n");
    exit(1);
  }
  
  short* audioClip = malloc(sizeof(short) * SAMPLESIZE);
  
  int numberOfTimesCrossingXAxis = 0;
  unsigned char writeByte 	 = 0;
  
  
  //1. iterate 8 times (downwards) for each bit
  //2. read 1 audioclip of size SampleSize
  //3. writeByte += pow(2, bit#)
  //4. write byte to file after each byte iteration
  //5. whatever youll figure it out
    
  
  for(int i = 0; i < transmissionInformation.frames/(SAMPLESIZE*8); i++){
    for(int j = 7; j >= 0; j--){      
      sf_read_short(inputWaveFile, audioClip, SAMPLESIZE);
      for(int k = 0; k < SAMPLESIZE-1; k++){	
	if((audioClip[k] < 0 && audioClip[k+1] > 0) || (audioClip[k] > 0 && audioClip[k+1] < 0)){	  
	  numberOfTimesCrossingXAxis++;	  
	}
      }
            
      
      if(numberOfTimesCrossingXAxis == 7){	      
	//printf("1\n");
	writeByte += pow(2,j);
      }
      else{
	//printf("0\n");
      }
            
      
      numberOfTimesCrossingXAxis = 0;
            
                
    }
    printf("%d\n", writeByte);
    putc(writeByte, outputFile);
    writeByte = 0;
  }
  fclose(outputFile);
}