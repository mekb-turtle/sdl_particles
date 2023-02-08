#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#define BG 0x1e, 0x1e, 0x2e
#define FG 0xf5, 0xc2, 0xe7
#define PARTICLE_MAX_LEN 400
#define MAGNITUDE 3.0
#define FPS 60.0
#define RADIUS 2
struct Particle {
	double x;
	double y;
	double vx;
	double vy;
	int ix;
	int iy;
	bool enabled;
} particles[PARTICLE_MAX_LEN];
bool bounds(double x, double y, int w, int h, int range) {
	return x >= -range && y >= -range && x < w + range && y < h + range;
}
bool in_range(double x1, double y1, double x2, double y2, int range) {
	double dx = x1-x2;
	double dy = y1-y2;
	if (dx < -range || dx > range || dy < -range || dy > range) return false;
	return sqrt(dx*dx+dy*dy) <= range;
}
void physics(int width, int height, int maxParticles, int range) {
	for (int i = 0; i < PARTICLE_MAX_LEN; ++i) {
		if (i >= maxParticles) { particles[i].enabled = false; continue; }
		if (!particles[i].enabled) {
			while (1) {
				if ((rand() & 0xff) > 0x7f) {
					particles[i].x = (rand() & 0xff) > 0x7f ? -range : (width - 1 + range);
					particles[i].y = (double)rand() / RAND_MAX * (height + range * 2) - range;
				} else {
					particles[i].x = (double)rand() / RAND_MAX * (width + range * 2) - range;
					particles[i].y = (rand() & 0xff) > 0x7f ? -range : (height - 1 + range);
				}
				double theta = (double)rand() / RAND_MAX * M_PI * 2;
				particles[i].vx = cos(theta) * MAGNITUDE;
				particles[i].vy = sin(theta) * MAGNITUDE;
				particles[i].x += particles[i].vx;
				particles[i].y += particles[i].vy;
				if (!bounds(particles[i].x, particles[i].y, width, height, range)) continue;
				particles[i].enabled = true;
				break;
			}
		} else {
			if (particles[i].vx == 0 && particles[i].vy == 0) {
				particles[i].enabled = false;
				continue;
			}
			particles[i].x += particles[i].vx;
			particles[i].y += particles[i].vy;
			if (!bounds(particles[i].x, particles[i].y, width, height, range)) particles[i].enabled = false;
		}
		particles[i].ix = (int) particles[i].x;
		particles[i].iy = (int) particles[i].y;
	}
}
int main() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	int width = 480;
	int height = 360;
	SDL_Window* win = SDL_CreateWindow("particles",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	width = 1920; // temporary resolution before we figure out window size
	height = 1080;
	int timeStart, frameTimeMSPF;
	double waitTime;
	int maxParticles;
	int range = 300;
	bool quit;
	SDL_Event event;
 	for (int i = 0; i < PARTICLE_MAX_LEN; ++i) {
		particles[i].enabled = false;
	}
	int started = 0;
	while (1) {
		timeStart = SDL_GetTicks();
		SDL_GetRendererOutputSize(rend, &width, &height);
		maxParticles = (width * height) / 20000;
		range = (width * height) / 20000 + 200;
		if (maxParticles > PARTICLE_MAX_LEN) maxParticles = PARTICLE_MAX_LEN;
		quit = false;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
				break;
			}
		}
		if (quit) break;
		SDL_SetRenderDrawColor(rend, BG, 0xff);
		SDL_RenderClear(rend);
		if (started < 10) {
			++started;
		} else {
			physics(width, height, maxParticles, range);
			SDL_SetRenderDrawColor(rend, FG, 0xff);
			for (int i = 0; i < PARTICLE_MAX_LEN; ++i) {
				if (!particles[i].enabled) continue;
				for (int j = 0; j < PARTICLE_MAX_LEN; ++j) {
					if (i == j) continue;
					if (!particles[j].enabled) continue;
					if (in_range(particles[i].ix, particles[i].iy, particles[j].ix, particles[j].iy, range)) {
						for (int x = -RADIUS; x <= RADIUS; ++x)
							for (int y = -RADIUS; y <= RADIUS; ++y) 
								if (in_range(x, y, 0, 0, RADIUS)) {
									SDL_RenderDrawLine(rend, particles[i].ix + x, particles[i].iy + y, particles[j].ix + x, particles[j].iy + y);
								}
					}
				}
			}
		}
		SDL_RenderPresent(rend);
		SDL_Delay(10);
		frameTimeMSPF = SDL_GetTicks() - timeStart;
		if ((double)frameTimeMSPF < 1000.0 / FPS) {
			waitTime = 1000.0 / FPS - (double)frameTimeMSPF;
			SDL_Delay((int)waitTime);
			frameTimeMSPF += waitTime;
		}
		printf("FPS: %i, MSPF: %i, W: %i, H: %i, magnitude: %i, range: %i\n", 1000 / frameTimeMSPF, frameTimeMSPF, width, height, maxParticles, range);
	}
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
