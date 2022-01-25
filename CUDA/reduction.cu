#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "util/util.h"

#include <stdio.h>
#include <stdlib.h>


#define N 5000
#define DIM 256			//taille de bloc impérativement en puissance de 2

//nvcc reduction.cu -o reduction
//./reduction

__global__ void sum(float *in, float *out, int n) {
	__shared__ float cache[DIM];
	int offset = blockIdx.x * blockDim.x; //début de la partie du tableau de ce bloc
	int i = threadIdx.x; //numéro du thread dans le bloc

	//copie du tableau dans la mémoire partagée
	if(offset + i < n) {
		cache[i] = in[offset + i];
	} else {
		cache[i] = 0;	// au cas ou on est a la fin du tableau ça change pas la somme
	}

	//réduction partielle dans la mémoire partagée
	int k = blockDim.x / 2;
	while(k > 0) {
		__syncthreads();	// a ne pas mettre dans le if sinon deadlock (tous les threads doivent le toucher)
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
	float a[N];
	float *dev_partial1, *dev_partial2, *tmp;

	for(int i = 0; i < N; i++) {
		a[i] = i + 1;
	}

	int n = N;
	int numBlocks = (n + DIM - 1) / DIM;
	HANDLE_CUDA_ERR(cudaMalloc(&dev_partial1, n * sizeof(float)));
	HANDLE_CUDA_ERR(cudaMalloc(&dev_partial2, numBlocks * sizeof(float)));
	HANDLE_CUDA_ERR(cudaMemcpy(dev_partial1, a, n * sizeof(float), cudaMemcpyHostToDevice));

	while(n > 1) {
		sum<<<numBlocks, DIM>>>(dev_partial1, dev_partial2, n);
		n = numBlocks;
		numBlocks = (n + DIM - 1) / DIM;
		tmp = dev_partial1;
		dev_partial1 = dev_partial2;
		dev_partial2 = tmp;
	}

	float sumGPU;
	HANDLE_CUDA_ERR(cudaMemcpy(&sumGPU, dev_partial1, sizeof(float), cudaMemcpyDeviceToHost));
	printf("Somme attendue : %.2f Somme obtenue : %.2f\n", N * (N + 1) / 2.0, sumGPU);

	HANDLE_CUDA_ERR(cudaFree(dev_partial1));
	HANDLE_CUDA_ERR(cudaFree(dev_partial2));
    return 0;
}