#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {
    int cycles, i;
    struct timespec timeStart, timeEnd, sleepStruct;
    unsigned long timeDuration = 0;
    FILE* fileStream;

    cycles = atoi(argv[1]);
    
    sleepStruct.tv_sec = 1;
    sleepStruct.tv_nsec = 0L;

    for (i = 0; i < cycles; ++i) {
	timeEnd.tv_nsec = 0; timeEnd.tv_sec = 0L;
	timeStart.tv_nsec = 0; timeStart.tv_sec = 0L;
	
        clock_gettime(CLOCK_MONOTONIC, &timeStart);
        clock_gettime(CLOCK_MONOTONIC, &timeEnd);
        timeDuration = (timeEnd.tv_nsec - timeStart.tv_nsec) + (timeEnd.tv_sec - timeStart.tv_sec)*1e9; // to get nanoseconds
        
	fileStream = fopen("result.txt", "a");
	fprintf(fileStream, "%ld\n", timeDuration);
	fclose(fileStream);
        nanosleep(&sleepStruct, NULL);
    }

}
