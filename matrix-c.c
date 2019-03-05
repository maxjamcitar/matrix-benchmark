// For now, this program works only on Linux due to some OS-dependent elements.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

float* multiplyMatrices (float* mat1, float* mat2, float* matRes, 
                         int matRows1, int matColumns1, int matRows2, int matColumns2) {
    for (int i = 0; i < matRows1; ++i) {
        for (int j = 0; j < matColumns2; ++j) {
            matRes[i*matColumns1 + j] = 0;
            for (int k = 0; k < matRows2; ++k) {
                matRes[i*matColumns1 + j] += mat1[i*matColumns1+k] * mat2[k*matColumns2+j];
            }
        }
    }
    return matRes;
}

void printMatrix(float* mat, int rows, int columns) 
{
    printf("\n");
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            printf ("%f\t", mat[i*columns+j]);
        }
        printf("\n");
    }
}

int main (int argc, char* argv[]) {
    int matDim = 1, cycles = 1;
    float *mat1, *mat2, *mat3;
    struct timespec timeStart, timeEnd;
    unsigned long timeDuration = 0;
    char fileName[256] = "matrix-output.txt";
    FILE* fileStream;
    long double resultFlops = 0;
    char* processorLine;
    char* processorName;
    char* processorFreqStrLine;
    char* processorFreqStrName;
    double processorFreqVal;
    FILE* outPipe;
    
    // Getting processor name
    processorLine = (char*)malloc(255);
    outPipe = popen("cat /proc/cpuinfo | grep 'model name' | uniq", "r"); // Linux dependent
    processorLine = fgets(processorLine, 255, outPipe);
    processorName = processorLine + 13;
    pclose(outPipe);

    // Getting processor frequency
    processorFreqStrLine = (char*)malloc(255);
    outPipe = popen("lscpu | grep max", "r"); // Linux dependent
    processorFreqStrLine = fgets(processorFreqStrLine, 255, outPipe);
    processorFreqStrName = processorFreqStrLine + 21;
    pclose(outPipe);
    processorFreqVal = atof(processorFreqStrName)*1e6;

    if (argc != 1) {
        matDim = atoi(argv[1]);
        cycles = atoi(argv[2]);
    }
    if (matDim <= 0 || cycles < 0) {
        fputs("ERROR: inappropriate external arguments.\n", stderr);
        return -1;
    }

    mat1 = (float*)malloc(sizeof(float)*matDim*matDim);
    mat2 = (float*)malloc(sizeof(float)*matDim*matDim);
    mat3 = (float*)malloc(sizeof(float)*matDim*matDim);

    for (int i = 0; i < matDim*matDim; ++i) {
        mat1[i] = (i+1.52)*0.12522*powl(-1.0, i+2);
        mat2[i] = (i-5.41)*2.41252*powl(-1.0,i+1);
        mat3[i] = 0;
    }

    printf ("Calculation started...\n");
    clock_gettime(CLOCK_MONOTONIC, &timeStart); // Linux dependent
    for (int k = 0; k < cycles; ++k) {
        // printMatrix(mat1, matDim, matDim);
        // printMatrix(mat2, matDim, matDim);
        mat3 = multiplyMatrices(mat1, mat2, mat3, matDim, matDim, matDim, matDim);
        // printMatrix(mat3, matDim, matDim);
    }
    clock_gettime(CLOCK_MONOTONIC, &timeEnd);
    timeDuration = (timeEnd.tv_sec - timeStart.tv_sec) * 1000000 + (timeEnd.tv_nsec - timeStart.tv_nsec) / 1000; // to get microseconds

    printf("%lu microseconds\n", timeDuration);

    free (mat1); free (mat2); free (mat3);

    resultFlops = (((long double)matDim*(long double)matDim*(long double)matDim) * (long double)cycles) / (((long double)timeDuration/1e6)*(long double)processorFreqVal*1e9);

    fileStream = fopen(fileName, "a");
    fprintf(fileStream, "%d\t%d\t%.10Lf\t%s\n", matDim, cycles, resultFlops, processorName);
    fclose(fileStream);

    free (processorLine);
    free (processorFreqStrLine);

    return 0;
}
