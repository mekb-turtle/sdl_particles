#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#define BG 0x1e, 0x1e, 0x2e, 0xff
#define FG 0xf5, 0xc2, 0xe7, 0xff
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define strerr strerror(errno)
#define PARTICLE_LEN 50
#define RANGE 300
struct Particle {
	double x;
	double y;
	double vx;
	double vy;
	int ix;
	int iy;
	bool a;
} particles[PARTICLE_LEN];
bool bounds(double x, double y, int w, int h) {
	return x >= -RANGE && y >= -RANGE && x < w + RANGE && y < h + RANGE;
}
bool in_range(double x1, double y1, double x2, double y2) {
	double dx = x1-x2;
	double dy = y1-y2;
	if (dx < -RANGE || dx > RANGE || dy < -RANGE || dy > RANGE) return false;
	return sqrt(dx*dx+dy*dy) <= RANGE;
}
int main() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window* win = SDL_CreateWindow("particles",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1000, 1000, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    while (1) {
		int width = 1000;
		int height = 1000;
		SDL_Event event;
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    	for (int i = 0; i < PARTICLE_LEN; ++i) {
			if (!particles[i].a) {
				while (1) {
					if ((rand() % 256) > 127) {
						particles[i].x = (rand() % 256) > 127 ? -RANGE : (width - 1 + RANGE);
						particles[i].y = (double)rand() / RAND_MAX * (height + RANGE * 2) - RANGE;
					} else {
						particles[i].x = (double)rand() / RAND_MAX * (width + RANGE * 2) - RANGE;
						particles[i].y = (rand() % 256) > 127 ? -RANGE : (height - 1 + RANGE);
					}
					double theta = (double)rand() / RAND_MAX * M_PI * 2;
					double mag = 1.5;
					particles[i].vx = cos(theta) * mag;
					particles[i].vy = sin(theta) * mag;
					particles[i].x += particles[i].vx;
					particles[i].y += particles[i].vy;
					if (!bounds(particles[i].x, particles[i].y, width, height)) continue;
					particles[i].a = true;
					break;
				}
			} else {
				particles[i].x += particles[i].vx;
				particles[i].y += particles[i].vy;
				if (!bounds(particles[i].x, particles[i].y, width, height)) particles[i].a = false;
			}
			particles[i].ix = (int) particles[i].x;
			particles[i].iy = (int) particles[i].y;
		}
		SDL_SetRenderDrawColor(rend, BG);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, FG);
    	for (int i = 0; i < PARTICLE_LEN; ++i) {
			SDL_RenderDrawPoint(rend, particles[i].ix, particles[i].iy);
    		for (int j = 0; j < PARTICLE_LEN; ++j) {
				if (in_range(particles[i].ix, particles[i].iy, particles[j].ix, particles[j].iy)) {
					SDL_RenderDrawLine(rend, particles[i].ix, particles[i].iy, particles[j].ix, particles[j].iy);
				}
			}
		}
		SDL_RenderPresent(rend);
		usleep(10000);
    }
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
	return 0;
}
