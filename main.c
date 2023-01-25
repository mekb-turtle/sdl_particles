#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#define BG 0x1e, 0x1e, 0x2e
#define FG 0xf5, 0xc2, 0xe7
#define TEXT_BG 0x31, 0x32, 0x44
#define TEXT_FG 0xcd, 0xd6, 0xf4
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define strerr strerror(errno)
#define PARTICLE_MAX_LEN 250
#define RANGE 300
#define MAGNITUDE 3.0
#define FPS 60.0
struct Particle {
	double x;
	double y;
	double vx;
	double vy;
	int ix;
	int iy;
	bool enabled;
} particles[PARTICLE_MAX_LEN];
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
    while (1) {
		timeStart = SDL_GetTicks();
		SDL_GetWindowSize(win, &width, &height);
		maxParticles = (width * height) / 20000;
		SDL_Event event;
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    	for (int i = 0; i < PARTICLE_MAX_LEN; ++i) {
			if (i >= maxParticles) { particles[i].enabled = false; continue; }
			if (!particles[i].enabled) {
				while (1) {
					if ((rand() & 0xff) > 0x7f) {
						particles[i].x = (rand() & 0xff) > 0x7f ? -RANGE : (width - 1 + RANGE);
						particles[i].y = (double)rand() / RAND_MAX * (height + RANGE * 2) - RANGE;
					} else {
						particles[i].x = (double)rand() / RAND_MAX * (width + RANGE * 2) - RANGE;
						particles[i].y = (rand() & 0xff) > 0x7f ? -RANGE : (height - 1 + RANGE);
					}
					double theta = (double)rand() / RAND_MAX * M_PI * 2;
					particles[i].vx = cos(theta) * MAGNITUDE;
					particles[i].vy = sin(theta) * MAGNITUDE;
					particles[i].x += particles[i].vx;
					particles[i].y += particles[i].vy;
					if (!bounds(particles[i].x, particles[i].y, width, height)) continue;
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
				if (!bounds(particles[i].x, particles[i].y, width, height)) particles[i].enabled = false;
			}
			particles[i].ix = (int) particles[i].x;
			particles[i].iy = (int) particles[i].y;
		}
		SDL_SetRenderDrawColor(rend, BG, 0xff);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, FG, 0xff);
    	for (int i = 0; i < PARTICLE_MAX_LEN; ++i) {
			if (!particles[i].enabled) continue;
    		for (int j = 0; j < PARTICLE_MAX_LEN; ++j) {
				if (!particles[j].enabled) continue;
				if (in_range(particles[i].ix, particles[i].iy, particles[j].ix, particles[j].iy)) {
					SDL_RenderDrawLine(rend, particles[i].ix, particles[i].iy, particles[j].ix, particles[j].iy);
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
		printf("FPS: %i, MSPF: %i\n", 1000 / frameTimeMSPF, frameTimeMSPF);
    }
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
	return 0;
}
