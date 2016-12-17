
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cstdlib>

// Things to do:
//   * Make food not spawn in snake
//   * Keep track of score
//   * Walls? Multiple foods?
//   * difficulty curve

struct Food {
	int x, y;

	void move() {
		x = rand() % (800 / 16);
		y = rand() % (800 / 16);
	}

	void draw(SDL_Renderer* renderer) {
		SDL_Rect r{ 16 * x, 16 * y, 16, 16 };
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderFillRect(renderer, &r);
	}
};

struct V2 {
	int x, y;
};

V2 operator+ (const V2& a, const V2& b) {
	return V2{ a.x + b.x, a.y + b.y };
}

void operator+= (V2& a, const V2& b) {
	a.x += b.x;
	a.y += b.y;
}

const int tail_max = 255;

struct Snake {
	V2 pos;
	V2 vel;
	V2 acc;

	V2 tail[tail_max];
	int tail_start, tail_end;
	int tail_len = 20;

	uint32_t accumulator;
	
	void update(uint32_t delta_time, struct Food& food) {
		accumulator += delta_time;
		if (accumulator > 100 - (tail_len)) {
			accumulator = 0;

			vel = acc;

			tail[tail_end % tail_max] = pos;

			tail_start++;
			tail_end++;

			pos.x += vel.x;
			pos.y += vel.y;

			pos.x = (pos.x + 50) % 50;
			pos.y = (pos.y + 50) % 50;

			if (pos.x == food.x && pos.y == food.y) {
				tail_len += 1;
				tail_start -= 1;
				food.move();
			}

			for (int i = 0; i < tail_len; i++) {
				V2& tail_pos = tail[(tail_start + i) % tail_max];
				if (tail_pos.x == pos.x && tail_pos.y == pos.y) {
					//we ded
					tail_len = 0;
					tail_start = tail_end;
				}
			}
		}
	}

	void draw(SDL_Renderer* renderer) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		for (int i = 0; i < tail_len; i++) {
			V2& tail_pos = tail[(tail_start + i) % tail_max];
			SDL_Rect r{ 16 * tail_pos.x, 16 * tail_pos.y, 16, 16 };
			SDL_RenderFillRect(renderer, &r);
		}

		SDL_Rect r{ 16 * pos.x, 16 * pos.y, 16, 16 };
		SDL_RenderFillRect(renderer, &r);
	}
};

int WinMain(int, int, const char*, int) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Snek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event e;

	bool running = true;
	Snake snake = {};
	Food food = {};
	food.move();

	uint32_t current_time = 0, previous_time, delta_time;

	while (running) {

		previous_time = current_time;
		current_time = SDL_GetTicks();
		delta_time = current_time - previous_time;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;

				case SDLK_w: if (snake.vel.y != 1) snake.acc = { 0, -1 }; break;
				case SDLK_s: if (snake.vel.y != -1) snake.acc = { 0, 1 }; break;
				case SDLK_a: if (snake.vel.x != 1) snake.acc = { -1, 0 }; break;
				case SDLK_d: if (snake.vel.x != -1) snake.acc = { 1, 0 }; break;
				}
			}
		}

		snake.update(delta_time, food);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		snake.draw(renderer);
		food.draw(renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}