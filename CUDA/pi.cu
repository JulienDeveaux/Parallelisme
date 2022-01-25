#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "util/util.h"

#include <stdio.h>
#include <stdlib.h>
// --- Timing includes
#include "GPUTiming/TimingCPU.h"
#include "GPUTiming/TimingGPU.cuh"
#include "GPUTiming/TimingCPU.cpp"
#include "GPUTiming/TimingGPU.cu"

#define N 1000000
#define DIM 256

__global__ void integrale(float *res, int n) {

    int i = blockIdx.x * blockDim.x + threadIdx.x;
    
    float h = 1.0/n;
    
    float x1 = i*h;
    float x2 = x1 + h;
    
    float y1 = sqrt(1-x1*x1);
    float y2 = sqrt(1-x2*x2);
    
    if(i < n) res[i] = (y1+y2)*h / 2;

}

__global__ void sum(float *in, float *out, int n) {
    __shared__ float cache[DIM];
    int offset = blockIdx.x * blockDim.x; //début de la partie du tableau de ce bloc
    int i = threadIdx.x; //numéro du thread dans le bloc

    //copie du tableau dans la mémoire partagée
    if(offset + i < n) {
        cache[i] = in[offset + i];
    } else {
        cache[i] = 0;   // au cas ou on est a la fin du tableau ça change pas la somme
    }

    //réduction partielle dans la mémoire partagée
    int k = blockDim.x / 2;
    while(k > 0) {
        __syncthreads();    // a ne pas mettre dans le if sinon deadlock (tous les threads doivent le toucher)
        if(i < k) {
            cache[i] += cache[i + k];
        }
        k /= 2;
    }

    //copie du résultat dans la mémoire globale
    if(i == 0) {
        out[blockIdx.x] = cache[0];
    }
}

int main(int argc, char const *argv[]) {
    TimingGPU timer_GPU;
    timer_GPU.StartCounter();

    float total;
    float *dev_res, *dev_res2, *tmp;

    //Allouer la mémoire device
    cudaMalloc(&dev_res, N * sizeof(float));
    cudaMalloc(&dev_res2, N * sizeof(float));
    cudaMalloc(&tmp, N * sizeof(float));
    
    //Kernel
    int numBlocks = (N+DIM-1) / DIM;
    integrale<<<numBlocks, DIM>>>(dev_res, N);

    int n = N;
    while(n > 1) {
        sum<<<numBlocks, DIM>>>(dev_res, dev_res2, n);
        n = numBlocks;
        numBlocks = (n + DIM - 1) / DIM;
        tmp = dev_res;
        dev_res = dev_res2;
        dev_res2 = tmp;
    }
    

    //Transfert de la mémoire vers host
    cudaMemcpy(&total, dev_res, sizeof(float), cudaMemcpyDeviceToHost);
    
    cudaFree(dev_res);
    cudaFree(dev_res2);

    printf("PI: %.9f\n\n", total*4);

    /*float res[N];
    float *dev_res;

    //Allouer la mémoire device
    cudaMalloc(&dev_res, N * sizeof(float));
    
    //Kernel
    int numBlocks = (N+DIM-1) / DIM;
    integrale<<<numBlocks, DIM>>>(dev_res, N);
    
    //Transfert de la mémoire vers host
    cudaMemcpy(res, dev_res, N*sizeof(float), cudaMemcpyDeviceToHost);    
    
    cudaFree(dev_res);
    
    //Resultats
    float total = 0.0;
    for(int i=0; i<N; i++) {total += res[i];}

    printf("PI: %.9f\n", total*4);*/

    printf("GPU Timing = %lf ms\n", timer_GPU.GetCounter());
    return 0;
}