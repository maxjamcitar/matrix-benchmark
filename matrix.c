// For now, this program works only on Linux due to some OS-dependent elements.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "Dependencies/OpenBLAS-run/include/cblas.h"

#ifdef __ARM_NEON
#include <arm_neon.h>
#elif defined __SSE__
#include <immintrin.h>
#else
#error "ERROR: intrinsics unsupported (need NEON or SSE)."
#endif

#define VECSIZE (4)

#ifdef __ARM_NEON
float64x2_t setq_float64 (float64_t arg1, float64_t arg2) {
	float64x2_t res;
	res = vsetq_lane_f64(arg1, res, 0);
	res = vsetq_lane_f64(arg2, res, 1);
	
	return res;
}
#endif

void microInit (double* mat, int m, int n, int ld) {
	int i, j;
	for (i = 0; i < m; ++i, mat += ld) {
#ifdef __ARM_NEON
		for (j = 0; j < n; j += 2) {
			vst1q_f64(mat + j, setq_float64(0.0, 0.0));
		}
#else
        for (j = 0; j < n; j += 4)
            _mm256_storeu_pd(mat + j, _mm256_setzero_pd());
	}
#endif
}

/*
void printf64 (float64x2_t arg, char* str) {
	double lane1 = vget_lane_f64(vget_low_f64(arg), 0);
	double lane2 = vget_lane_f64(vget_high_f64(arg), 0);
	printf("%s\n", str);
	printf ("lane1 = %lf, lane2 = %lf\n", lane1, lane2);
}
*/

void micro_4x4 (double* mat1, double* mat2, double* matRes, int ld1, int ld2, int ld3, int matColumns1) {
	int i;
#ifdef __ARM_NEON
	float64x2_t m1_0, m1_1, m2_0, m2_1;
	float64x2_t mr_00 = setq_float64(0.0, 0.0);
	float64x2_t mr_01 = setq_float64(0.0, 0.0);
	float64x2_t mr_10 = setq_float64(0.0, 0.0);
	float64x2_t mr_11 = setq_float64(0.0, 0.0);
	float64x2_t mr_20 = setq_float64(0.0, 0.0);
	float64x2_t mr_21 = setq_float64(0.0, 0.0);
	float64x2_t mr_30 = setq_float64(0.0, 0.0);
	float64x2_t mr_31 = setq_float64(0.0, 0.0);

	microInit(matRes, 4, 4, ld3); 
	for (i = 0; i < matColumns1; ++i) {
		m1_0 = setq_float64(mat1[0], 	   mat1[0]);		// mat1[0][i]=m1_0 twice
		m1_1 = setq_float64(mat1[0 + ld1], mat1[0 + ld1]);	// mat1[0][i]=m1_0 twice

		m2_0 = vld1q_f64(mat2 + 0);				// mat2[i][0]=m2_0[0], mat2[i][1]=m2_0[1]
		m2_1 = vld1q_f64(mat2 + 2);				// mat2[i][2]=m2_1[0], mat2[i][2]=m2_1[1]

		// fused multiply-add of mat1 and mat2 vectors
		mr_00 = vfmaq_f64(mr_00, m1_0, m2_0);
		mr_01 = vfmaq_f64(mr_01, m1_0, m2_1);
		mr_10 = vfmaq_f64(mr_10, m1_1, m2_0);
		mr_11 = vfmaq_f64(mr_11, m1_1, m2_1);
		
		m1_0 = setq_float64(mat1[0 + ld1*2], 	   mat1[0 + ld1*2]);		// mat1[0][i]=m1_0 twice
		m1_1 = setq_float64(mat1[0 + ld1*3], mat1[0 + ld1*3]);	// mat1[0][i]=m1_0 twice

		mr_20 = vfmaq_f64(mr_20, m1_0, m2_0);
		mr_21 = vfmaq_f64(mr_21, m1_0, m2_1);
		mr_30 = vfmaq_f64(mr_30, m1_1, m2_0);
		mr_31 = vfmaq_f64(mr_31, m1_1, m2_1);

		++mat1;
		mat2 += ld2; // incrementing matrices
	}
	/*
	vst1q_f64(matRes + 0, 		vaddq_f64(mr_00, setq_float64(matRes[0], matRes[0])));	
	vst1q_f64(matRes + 2, 		vaddq_f64(mr_01, setq_float64(matRes[2], matRes[2])));	
	vst1q_f64(matRes + ld3*1 + 0, 	vaddq_f64(mr_10, setq_float64(matRes[ld3*1 + 0], matRes[ld3*1 + 0])));	
	vst1q_f64(matRes + ld3*1 + 2, 	vaddq_f64(mr_11, setq_float64(matRes[ld3*1 + 2], matRes[ld3*1 + 2])));	
	vst1q_f64(matRes + ld3*2 + 0, 	vaddq_f64(mr_20, setq_float64(matRes[ld3*2 + 0], matRes[ld3*2 + 0])));	
	vst1q_f64(matRes + ld3*2 + 2, 	vaddq_f64(mr_21, setq_float64(matRes[ld3*2 + 2], matRes[ld3*2 + 2])));	
	vst1q_f64(matRes + ld3*3 + 0, 	vaddq_f64(mr_30, setq_float64(matRes[ld3*3 + 0], matRes[ld3*3 + 0])));	
	vst1q_f64(matRes + ld3*3 + 2, 	vaddq_f64(mr_31, setq_float64(matRes[ld3*3 + 2], matRes[ld3*3 + 2])));	
	*/
	vst1q_f64(matRes + 0, 		mr_00);	
	vst1q_f64(matRes + 2, 		mr_01);	
	vst1q_f64(matRes + ld3*1 + 0, 	mr_10);	
	vst1q_f64(matRes + ld3*1 + 2, 	mr_11);	
	vst1q_f64(matRes + ld3*2 + 0, 	mr_20);	
	vst1q_f64(matRes + ld3*2 + 2, 	mr_21);	
	vst1q_f64(matRes + ld3*3 + 0, 	mr_30);	
	vst1q_f64(matRes + ld3*3 + 2, 	mr_31);	
#else
    __m256d m1_0, m1_1, m2_0;

    __m256d mr_00 = _mm256_setzero_pd();
    __m256d mr_01 = _mm256_setzero_pd();
    __m256d mr_10 = _mm256_setzero_pd();
    __m256d mr_11 = _mm256_setzero_pd();
    __m256d mr_20 = _mm256_setzero_pd();
    __m256d mr_21 = _mm256_setzero_pd();
    __m256d mr_30 = _mm256_setzero_pd();
    __m256d mr_31 = _mm256_setzero_pd();

	microInit(matRes, 4, 4, ld3); 
    for (i = 0; i < matColumns1; i++)
    {
        m2_0 = _mm256_loadu_pd(mat2 + 0);
        m1_0 = _mm256_set1_pd(mat1[0]);
        m1_1 = _mm256_set1_pd(mat1[0 + ld1]);
        mr_00 = _mm256_fmadd_pd(m1_0, m2_0, mr_00);
        mr_01 = _mm256_fmadd_pd(m1_1, m2_0, mr_01);
        m1_0 = _mm256_set1_pd(mat1[0 + ld1*2]);
        m1_1 = _mm256_set1_pd(mat1[0 + ld1*3]);
        mr_10 = _mm256_fmadd_pd(m1_0, m2_0, mr_10);
        mr_11 = _mm256_fmadd_pd(m1_1, m2_0, mr_11);

	++mat1;
	mat2 += ld2; // incrementing matrices
    }

    _mm256_storeu_pd(matRes + 0,        mr_00);
    _mm256_storeu_pd(matRes + ld3,      mr_01);
    _mm256_storeu_pd(matRes + ld3*2,    mr_10);
    _mm256_storeu_pd(matRes + ld3*3,    mr_11);
#endif
}

void myDgemm (double* mat1, double* mat2, double* matRes, int matRows1, int matColumns1, int matRows2, int matColumns2) {
    int i, j;
    for (i = 0; i < matRows1; i += 4) {
	for (j = 0; j < matColumns2; j += 4) {
		micro_4x4(mat1 + i*matColumns1, mat2 + j, matRes + i*matColumns2 + j, matColumns1, matColumns2, matColumns2, matColumns1);
	}
    }
}

int makeDimMalloc (int matDim) {
    int wholes = 0;
    if (matDim % VECSIZE == 0)
        wholes=matDim*matDim;
    else {
	wholes = (matDim / VECSIZE + 1)*VECSIZE;
	wholes *= wholes;
    }
    return wholes;
}

void printMatrix(double* mat, int rows, int columns) 
{
    int i, j;
    printf("\n");
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < columns; ++j) {
            printf ("%f\t", mat[i*columns+j]);
        }
        printf("\n");
    }
    printf ("\n");
}

int main (int argc, char* argv[]) {
    int i, k;
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

    int matDimMalloc = makeDimMalloc(matDim);
    mat1 = (double*)malloc(sizeof(double)*matDimMalloc);
    mat2 = (double*)malloc(sizeof(double)*matDimMalloc);
    mat3 = (double*)malloc(sizeof(double)*matDimMalloc);

    /*
    for (i = 0; i < matDim*matDim; ++i) {
        mat1[i] = (i+1.52)*0.12522*pow(-1.0, i+2);
        mat2[i] = (i-5.41)*2.41252*pow(-1.0,i+1);
        mat3[i] = 0;
    }
    */
    
    for (i = 0; i < matDim*matDim; ++i) {
        mat1[i] = i;
        mat2[i] = i;
        mat3[i] = 0;
    }
    

    timeStart.tv_nsec = 0;  timeStart.tv_sec = 0;
    timeEnd.tv_nsec = 0;    timeEnd.tv_sec = 0;

    if (strcmp(dgemmArg, "dgemm") == 0) {
        printf("Calculation with dgemm mode started.\n");        
        strcpy(fileName, "text-output/matrix-output-dgemm.txt");
        clock_gettime(CLOCK_MONOTONIC, &timeStart); // Linux dependent
        for (k = 0; k < cycles; ++k) {
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
        for (k = 0; k < cycles; ++k) {
            printMatrix(mat1, matDim, matDim);
            printMatrix(mat2, matDim, matDim);
            myDgemm(mat1, mat2, mat3, matDim, matDim, matDim, matDim);
            printMatrix(mat3, matDim, matDim);
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
