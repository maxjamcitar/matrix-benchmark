// For now, this program works only on Linux due to some OS-dependent elements.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "Dependencies/OpenBLAS-run/include/cblas.h"

#define BUNCHSIZE (5)

double* multiplyMatrices_default (double* mat1, double* mat2, double* matRes, 
                         int matRows1, int matColumns1, int matRows2, int matColumns2) {
    double* mr;
    double m1;
    double* m2;
    for (int i = 0; i < matRows1; ++i) {
        mr = matRes + i*matColumns1;
        for (int j = 0; j < matRows2; ++j) {
            mr[j] = 0;
        }
        for (int k = 0; k < matColumns2; ++k) {
            m1 = mat1[i*matColumns1+k];
            m2 = mat2 + k*matColumns2;
            for (int j = 0; j < matRows2; ++j) {
                mr[j] += m1 * m2[j]; // 2 FLOP
            }
        }
    }
    return matRes;
}

// Implementation of unrolling is yet to be done
double* multiplyMatrices_unrolled5 (double* mat1, double* mat2, double* matRes, 
                         int matRows1, int matColumns1, int matRows2, int matColumns2) {
    for (int i = 0; i < matRows1; ++i) {
        for (int k = 0; k < matColumns2; ++k) {
            for (int j = 0; j < matRows2; ++j) {
                matRes[i*matColumns1 + j] = matRes[i*matColumns1 + j] + mat1[i*matColumns1+k] * mat2[k*matColumns2+j]; // 2 FLOP
            }
        }
    }
    return matRes;
}

double* multiplyMatrices (double* mat1, double* mat2, double* matRes, int matRows1, int matColumns1, int matRows2, int matColumns2) {
    // if (matRows2 >= BUNCHSIZE)
    //    multiplyMatrices_unrolled5 (mat1, mat2, matRes, matRows1, matColumns1, matRows2, matColumns2);
    // else
        multiplyMatrices_default (mat1, mat2, matRes, matRows1, matColumns1, matRows2, matColumns2);
    return matRes;
}

void printMatrix(double* mat, int rows, int columns) 
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
    double *mat1, *mat2, *mat3;
    struct timespec timeStart, timeEnd;
    unsigned long timeDuration = 0;
    char fileName[256] = "text-output/matrix-output-def.txt";
    FILE* fileStream;
    long double resultFlops = 0;
    char* processorLine;
    char* processorName;
    char* processorFreqStrLine;
    char* processorFreqStrName;
    char* dgemmArg;
    double processorFreqVal;
    FILE* outPipe;
    
    dgemmArg = (char*)malloc(255);
    strcpy(dgemmArg, "def");

    // Getting processor name
    processorLine = (char*)malloc(255);
    outPipe = popen("cat /proc/cpuinfo | grep 'model name' | uniq", "r"); // Linux dependent
    processorLine = fgets(processorLine, 255, outPipe);
    processorName = processorLine + strlen("model name	: ");
    pclose(outPipe);

    // Getting processor frequency
    processorFreqStrLine = (char*)malloc(255);
    outPipe = popen("lscpu | grep max", "r"); // Linux dependent
    processorFreqStrLine = fgets(processorFreqStrLine, 255, outPipe);
    processorFreqStrName = processorFreqStrLine + strlen("CPU max MHz:         ");
    pclose(outPipe);
    processorFreqVal = atof(processorFreqStrName)*1e6;

    if (argc != 1) {
        matDim = atoi(argv[1]);
        cycles = atoi(argv[2]);
        if (argc >= 4)
            strcpy(dgemmArg, argv[3]);
    }
    if (matDim <= 0 || cycles < 0) {
        fputs("ERROR: inappropriate external arguments.\n", stderr);
        return -1;
    }

    mat1 = (double*)malloc(sizeof(double)*matDim*matDim);
    mat2 = (double*)malloc(sizeof(double)*matDim*matDim);
    mat3 = (double*)malloc(sizeof(double)*matDim*matDim);

    for (int i = 0; i < matDim*matDim; ++i) {
        mat1[i] = (i+1.52)*0.12522*pow(-1.0, i+2);
        mat2[i] = (i-5.41)*2.41252*pow(-1.0,i+1);
        mat3[i] = 0;
    }

    timeStart.tv_nsec = 0;  timeStart.tv_sec = 0;
    timeEnd.tv_nsec = 0;    timeEnd.tv_sec = 0;

    if (strcmp(dgemmArg, "dgemm") == 0) {
        printf("Calculation with dgemm mode started.\n");        
        strcpy(fileName, "text-output/matrix-output-dgemm.txt");
        clock_gettime(CLOCK_MONOTONIC, &timeStart); // Linux dependent
        for (int k = 0; k < cycles; ++k) {
            //printMatrix(mat1, matDim, matDim);
            //printMatrix(mat2, matDim, matDim);
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, matDim, matDim, matDim, 1, mat1, matDim, mat2, matDim, 0, mat3, matDim);
            //printMatrix(mat3, matDim, matDim);
        }
        clock_gettime(CLOCK_MONOTONIC, &timeEnd);
    }
    else {
        printf("Calculation with default mode started.\n");        
        clock_gettime(CLOCK_MONOTONIC, &timeStart); // Linux dependent
        for (int k = 0; k < cycles; ++k) {
            // printMatrix(mat1, matDim, matDim);
            // printMatrix(mat2, matDim, matDim);
            mat3 = multiplyMatrices(mat1, mat2, mat3, matDim, matDim, matDim, matDim);
            // printMatrix(mat3, matDim, matDim);
        }
        clock_gettime(CLOCK_MONOTONIC, &timeEnd);
    }

    timeDuration = (timeEnd.tv_sec - timeStart.tv_sec) * 1000000 + (timeEnd.tv_nsec - timeStart.tv_nsec) / 1000; // to get microseconds

    printf("%lu microseconds\n", timeDuration);

    free (mat1); free (mat2); free (mat3);

    // Counting FLOPS
    resultFlops = (((2*(long double)matDim*(long double)matDim*(long double)matDim) * (long double)cycles) 
    / ((long double)timeDuration/1e6)) / (long double)processorFreqVal;

    // Creating output folder
    // As GCC complains about return values and we don't care about 'system's one,
    // warning check is temporarily disabled.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result" 
    system ("mkdir -p text-output");
#pragma GCC diagnostic pop 

    fileStream = fopen(fileName, "a");
    if (fileStream != NULL) {
        fprintf(fileStream, "%d\t%d\t%.10Lf\t%s\n", matDim, cycles, resultFlops, processorName);
        fclose(fileStream);
    }
    else {
        printf ("ERROR: can't open file '%s'.\n", fileName);
    }

    free (processorLine);
    free (processorFreqStrLine);
    free (dgemmArg);

    return 0;
}
