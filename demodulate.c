/*Collin Oswalt - 4/21/15
 * 
 * New version of demodulate with better algorithm.
 * Only reads one sample at a time since it's foolish
 * to assume that a cassette recorder operates at a perfect
 * 74 sample rate
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <math.h>

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
  
    /*only get one sample at a time and check if the adjacent sample is of reversed sign
     * then, take the number of samples it took before the sign was reversed. if it is >3 but <14,
     * add 1 bit to the current number place. if it is >=14, do nothing (add 0)*/          
  short thisSample 			= 0;
    /*used for debugging; 44100/totalSamples = current timestamp along the audio file*/
  int totalSamples			= 0;
    /*number of times sign has changed for current wave; add 1 when # of 2400hz periods = 8
     add 0 when # of 1200hz periods = 4*/
  short numberOf1200hzHalfPeriods	= 0;  
  short numberOf2400hzHalfPeriods	= 0;
    
    /*start positive; transmission starts positive.*/
  short lastSample			= 1;
    /*number of samples read before sign was switched*/
  short numberOfSamplesBeforeSwitch 	= 0;
    /*number placed to be added per bit. decreases every time number is added*/
  char numberPlaceForThisByte 		= 7;
  
    /*byte to be written*/
  char writeByte 			= 0;
  
  for(int i = 0; i < transmissionInformation.frames; i++){    
      /*get next sample*/
    sf_read_short(inputWaveFile, &thisSample, 1);  
      /*used for debugging*/
    totalSamples++;
      /*check if sign has switched*/
    if(thisSample == 0 || (thisSample < 0 && lastSample > 0) || (thisSample > 0 && lastSample < 0)){
	// old output debugging
      //printf("Triggered! this: %d\tlast: %d\n",thisSample, lastSample);      
	
	/*number of samples between 3 and 14, is one. (a perfect 1 is 8.5 samples and
	 * a perfect 0 is 17 samples; 17 - 8 = 9. 9/2 = 4.5 (rounded to 5. 9 - 5 = 4, 9 + 5 = 14.
	 * Thus, the margin of error is +- 5*/
      if(numberOfSamplesBeforeSwitch > 3 && numberOfSamplesBeforeSwitch < 14){
	numberOf2400hzHalfPeriods++;
	  /*reset sample count*/
	numberOfSamplesBeforeSwitch = 0;
      }
      else if(numberOfSamplesBeforeSwitch >= 14 && numberOfSamplesBeforeSwitch <= 24){
	numberOf1200hzHalfPeriods++;
	numberOfSamplesBeforeSwitch = 0;
      }      
    }
      //more old debugging stuff
    //printf("this: %d last: %d at s: %f\n", thisSample, lastSample, (double)(totalSamples)/(double)(44100));
    //printf("%d\n",numberOfSamplesBeforeSwitch);
    //printf("%d - %d\n", numberOf2400hzHalfPeriods,numberOf1200hzHalfPeriods);
    
      /*# of half periods should be 8 (for 4 total periods)*/
    if(numberOf2400hzHalfPeriods == 8){
      //add 1 bit      
      
	/*reset metrics*/
      numberOf1200hzHalfPeriods = 0;
      numberOf2400hzHalfPeriods = 0;
	/*add 1 to current number place*/
      writeByte += pow(2,numberPlaceForThisByte);
	/*iterate to next number place*/
      numberPlaceForThisByte--;
    }
      /*# of half periods should be 4 (for 2 total periods*/
    if(numberOf1200hzHalfPeriods == 4){
      //add 0 bit (or do nothing really)
	/*reset metrics*/
      numberOf1200hzHalfPeriods = 0; 
      numberOf2400hzHalfPeriods = 0;
	/*iterate to number place. write nothing to current bit*/
      numberPlaceForThisByte--;
    }
    
      //debugging
    //printf("byte so far: %d at number place %d\n", writeByte, numberPlaceForThisByte);
    
    if(numberPlaceForThisByte == -1){
	//debugging
      //printf("byte written!\n");
      
	/*write byte to file stream*/
      putc(writeByte, outputFile);   
	/*reset byte*/
      writeByte = 0;
	/*reset number place*/
      numberPlaceForThisByte = 7;
    }
    
      /*iteratre number of samples*/
    numberOfSamplesBeforeSwitch++;
      
      /*get previous sample for comparing signs with next sample*/
    lastSample = thisSample;
    
  }
    /*write output*/
  fclose(outputFile);
}
