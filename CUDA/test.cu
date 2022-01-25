#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000
#define DIM 256

__global__ void calc(float *x, float *y, float *res) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if(x[index]*x[index]+y[index]*y[index] <= 1) {
        res[0]++;
    }
}

int main()
{
    printf("pi");
    float pi, x[N], y[N], res[1];
    float *dev_x, *dev_y, *dev_res;
    srand(time(NULL));

    for(int i = 0; i < N - 1; i++) {
        printf("%d\n", i);
        x[i] = rand() % 1;
        y[i] = rand() % 1;
    }

    cudaMalloc(&dev_x, N*sizeof(float));
    cudaMalloc(&dev_y, N*sizeof(float));
    cudaMalloc(&dev_res, sizeof(float));

    cudaMemcpy(dev_x, x, N*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_y, y, N*sizeof(float), cudaMemcpyHostToDevice);

    calc<<<N / DIM, DIM>>>(dev_x, dev_y, dev_res);

    cudaMemcpy(res, &dev_res, sizeof(float), cudaMemcpyDeviceToHost);

    pi = res[0]/N*4;
    printf("pi : %f\n", pi);

    cudaFree(dev_x);
    cudaFree(dev_y);
    cudaFree(dev_res);

    return 0;
}//https://www.deleze.name/marcel/sec2/cours/CalculIntegral/pi_calcul.pdf