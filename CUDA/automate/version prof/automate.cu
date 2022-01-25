#include "common/book.h"
#include "common/gpu_anim.h"

#define WIDTH 800
#define HEIGHT 600
#define DIM 16
#define M 200
#define K 3
#define G 28

//nvcc heat.cu -o heat -lglut -lGL -lGLU
//__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia ./automate

__global__ void color(int *t, uchar4 *buf) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	if (x < WIDTH && y < HEIGHT) {
		int offset = y * WIDTH + x;
		
		float r, g, b;
		if(t[offset] > (M / 3) && t[offset] < (2*M / 3)) {	// M entre 1/3 et 2/3
			r = 100;
			g = 100;
			b = 100;
		} else if(t[offset] > (2*M / 3)) {					// M entre 2/3 et 3/3
			r = 200;
			g = 200;
			b = 200;
		} else {
			r = 0;
			g = 0;
			b = 0;
		}
		buf[offset].x = r;
		buf[offset].y = g;
		buf[offset].z = b;
		buf[offset].w = 255;		
	}
}

__global__ void diffuse(int *t_current, int *t_next) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	if (x < WIDTH && y < HEIGHT) {
		int offset = y * WIDTH + x;
		int top = y == HEIGHT - 1 ? offset : offset + WIDTH;
		int bottom = y == 0 ? offset : offset - WIDTH;
		int left = x == 0 ? offset : offset - 1;
		int right = x == WIDTH - 1 ? offset : offset + 1;
		int topRight = offset;
		int topLeft = offset;
		int bottomRight = offset;
		int bottomLeft = offset;
		if(y != HEIGHT && x != WIDTH) {
			topRight = offset + WIDTH + 1;
		}
		if(y != HEIGHT && x != 0) {
			topLeft = offset + WIDTH - 1;
		}
		if(y != 0 && x != WIDTH) {
			bottomRight = offset - WIDTH + 1;
		}
		if(y != 0 && x != 0) {
			bottomLeft = offset - WIDTH - 1;
		}

    	int nbNonSain = 0;
    	int moy = 0;
    	if(t_current[top] != 0) {
    		nbNonSain++;
    		moy += t_current[top];
    	}
    	if(t_current[bottom] != 0) {
    		nbNonSain++;
    		moy += t_current[bottom];
    	}
    	if(t_current[left] != 0) {
    		nbNonSain++;
    		moy += t_current[left];
    	}
    	if(t_current[right] != 0) {
    		nbNonSain++;
    		moy += t_current[right];
    	}
    	if(t_current[topRight] != 0) {
    		nbNonSain++;
    		moy += t_current[topRight];
    	}
    	if(t_current[topLeft] != 0) {
    		nbNonSain++;
    		moy += t_current[topLeft];
    	}
    	if(t_current[bottomLeft] != 0) {
    		nbNonSain++;
    		moy += t_current[bottomLeft];
    	}
    	if(t_current[bottomRight] != 0) {
    		nbNonSain++;
    		moy += t_current[bottomRight];
    	}


		if(t_current[offset] == M) { 					//si la cellule est malade elle devient saine

    		t_next[offset] = 0;

    	} else if (t_current[offset] == 0) {			//si elle est saine, (#voisins non sains)/K)

			t_next[offset] = nbNonSain / K;

    	} if (t_current[offset] != 0 && t_current[offset] != M) {	//si la cellule est infectée

    		t_next[offset] = ((t_current[offset]+moy) / (nbNonSain+1)) + G;

    	}

    	if(t_next[offset] > M) {
    		t_next[offset] = M;
    	}
	}
}

struct Data {
	int *t1;
	int *t2;
	dim3 blocks;
	dim3 threads;
};

void render_callback(uchar4 *buf, Data *data, int ticks) {
	diffuse<<<data->blocks, data->threads>>>(data->t1, data->t2);
	diffuse<<<data->blocks, data->threads>>>(data->t2, data->t1);
	color<<<data->blocks, data->threads>>>(data->t1, buf);
}

void clean_callback(Data *data) {
	HANDLE_ERROR(cudaFree(data->t1));
	HANDLE_ERROR(cudaFree(data->t2));
}

int main() {
	Data data;
	GPUAnimBitmap bitmap(WIDTH, HEIGHT, &data);
		
	size_t size = WIDTH * HEIGHT * sizeof(int);
	int *t_initial = (int *)calloc(WIDTH * HEIGHT, sizeof(int));

	srand(time(NULL));
	for(int i = 0; i < WIDTH * HEIGHT; i++) {
		int r = rand() % M+1;
		t_initial[i] = r;
	}
	
	data.blocks = dim3((WIDTH + DIM - 1) / DIM, (HEIGHT + DIM - 1) / DIM);
	data.threads = dim3(DIM, DIM);
	HANDLE_ERROR(cudaMalloc(&data.t1, size));
	HANDLE_ERROR(cudaMalloc(&data.t2, size));
	HANDLE_ERROR(cudaMemcpy(data.t1, t_initial, size, cudaMemcpyHostToDevice));
	
	bitmap.anim_and_exit((void (*)(uchar4*, void*, int))render_callback, (void (*)(void*))clean_callback);
	return 0;
}

