#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "util/util.h"
#include "util/gpu_bitmap.h"
#include <GL/freeglut.h>

#include <stdio.h>
#include <stdlib.h>


#define N 5000
#define WIDTH 800
#define HEIGHT 600
#define DIM 16
#define K 1.0e-1f


//nvcc diffusion.cu -o -lglut -DREPORT_FPS -lGL diffusion
//./diffusion

struct Data {
	float *t1, *t2; // tableaux températures
	dim3 blocks, threads; // dimensions de la grille
};

__global__ void diffuse(float *t_current, float *t_next) { // un pas de temps
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	if(x < WIDTH && y < HEIGHT) {
		int offset = y * WIDTH + x;
		int top = y == HEIGHT - 1 ? offset : offset + WIDTH;
		int bottom = y == 0 ? offset : offset - WIDTH;
		int left = x == 0 ? offset : offset - 1;
		int right =x == WIDTH - 1 ? offset : offset + 1;

		t_next[offset] = (1 - 4 * K) * t_current[offset] + K * (t_current[top] +
			t_current[bottom] + t_current[left] + t_current[right]);
	}
}

__global__ void color(float *t, uchar4 *buf) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	if(x < WIDTH && y < HEIGHT) {
		int offset = y * WIDTH + x;
		float t3 = 3 * t[offset];
		float r, g, b;
		if(t3 < 1) {
			r = t3 * 255;
			g = 0;
			b = 0;
		} else if(t3 < 2) {
			r = 255;
			g = (t3 - 1) * 255;
			b = 0;
		} else {
			r = 255;
			g = 255;
			b = (t3 - 2) * 255;
		}
		buf[offset].x = r;
		buf[offset].y = g;
		buf[offset].z = b;
		buf[offset].w = 255;
	}
}

void render_callback(uchar4 *buf, Data *data, int ticks)  {
	diffuse<<<data->blocks, data->threads>>>(data->t1, data->t2);
	diffuse<<<data->blocks, data->threads>>>(data->t2, data->t1);
	color<<<data->blocks, data->threads>>>(data->t1, buf);
}

void clean_callback(Data *data) {
	HANDLE_CUDA_ERR(cudaFree(data->t1));
	HANDLE_CUDA_ERR(cudaFree(data->t2));
}

int main() {
	Data data;
	GPUBitmap bitmap(WIDTH, HEIGHT, &data, "Heat");

	size_t size = WIDTH * HEIGHT * sizeof(float);
	float *t_initial = (float*)malloc(size);
	//init la temp de chaque cellule

	//init data
	data.blocks = dim3((WIDTH + DIM - 1) / DIM, (HEIGHT + DIM - 1) / DIM);
	data.threads = dim3(DIM, DIM);
	HANDLE_CUDA_ERR(cudaMalloc(&data.t1, size));
	HANDLE_CUDA_ERR(cudaMalloc(&data.t2, size));
	HANDLE_CUDA_ERR(cudaMemcpy(data.t1, t_initial, size, cudaMemcpyHostToDevice));

	//lancer la boucle d'annimation
	bitmap.animate((void (*)(uchar4*, void*, int))render_callback, (void (*)(void*))clean_callback);
}