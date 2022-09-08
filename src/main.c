#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/rtc.h>

#include <stdbool.h>

//Size of individual sprites and snake tiles
#define TILE_SIZE 16

//Game speed
#define TICK_RATE 20

#define STATE_MENU 0
#define STATE_GAME 1

//Amount of spawning apples
#define MAX_FOOD 3

struct Segment {
	int x;
	int y;

	struct Segment *prev;
};

//Snakes head
struct Segment snake;
//Ptr to snakes tail
struct Segment *lastSegment;

//Snakes moving direction
int direction = 1;

//Array of food locations
struct Segment *food;

int score = 0;

int state = STATE_MENU;
int lastTick;

int MAX_X, MAX_Y;

//Random number generator
int lastRand = 0;

int randint() {
	lastRand = 0x41C64E6D * lastRand + 0x3039;
	return lastRand;
}

void addSegment() {
	struct Segment* seg = sys_malloc(sizeof(struct Segment));

	seg->x = lastSegment->x;
	seg->y = lastSegment->y;

	seg->prev = lastSegment;

	//TODO: this is dumb
	struct Segment* i = lastSegment;
	for (; i->prev != lastSegment; i = i->prev);
	i->prev = seg;

	lastSegment = seg;

	score++;
}

void drawSquare(struct Segment* pos, int size, color_t color) {
	const int halfSize = size / 2;
	for (int x = pos->x - halfSize; x < pos->x + halfSize; x++) {
		for (int y = pos->y - halfSize; y < pos->y + halfSize; y++) {
			Bdisp_SetPoint_VRAM(x, y, color);
		}
	}
}

void render() {
	for (int i = 0; i < MAX_FOOD; i++) {
		drawSquare(&food[i], TILE_SIZE, COLOR_RED);
	}

	drawSquare(&snake, TILE_SIZE, COLOR_BLUE);

	struct Segment *seg = lastSegment;
	do {
		drawSquare(seg, TILE_SIZE, COLOR_GREEN);
		seg = seg->prev;
	} while (seg != lastSegment);
}

bool input() {
	short unsigned key;
	int column, row;

	int keyPressType = GetKeyWait_OS(&column, &row, KEYWAIT_HALTOFF_TIMEROFF, 0, 0, &key);

	if (keyPressType == KEYREP_KEYEVENT) {
		if ((column == 0x07) && (row == 0x0A)) {
			if (state != STATE_GAME) {
				state = STATE_GAME;

				//Hide the hud
				EnableStatusArea(3);

				lastRand = RTC_GetTicks();

				int startPos = TILE_SIZE * 4;

				snake.x = startPos;
				snake.y = startPos;

				lastSegment = sys_malloc(sizeof(struct Segment));
				lastSegment->prev = lastSegment;

				food = sys_malloc(sizeof(struct Segment) * MAX_FOOD);

				for (int i = 0; i < MAX_FOOD; i++) {
					food[i].x = ((randint() % (MAX_X - 3)) + 1) * TILE_SIZE;
					food[i].y = ((randint() % (MAX_Y - 3)) + 1) * TILE_SIZE;
				}
			}
		} else if (state == STATE_GAME) {
			//Arrow keys
			int hash = row * 0xFF + column;

			switch (hash) {
				case 0x7F8 + 0x02:
					direction = 0;
				break;
				case 0x7F8 + 0x03:
					direction = 1;
				break;
				case 0x8F7 + 0x03:
					direction = 2;
				break;
				case 0x8F7 + 0x02:
					direction = 3;
				break;
			}
		}

		if ((column == 0x04) && (row == 0x09)) {
			return true;
		}
	}

	return false;
}

//Returns whether snakes head hit the body
bool collidesWithSelf() {
	//Works because the snake can only collide with itself if its length is above 3
	for (struct Segment *seg = lastSegment; seg->prev != lastSegment; seg = seg->prev) {
			if ((snake.x == seg->x) && (snake.y == seg->y)) {
			return true;
		}
	}

	return false;
}

//Returns food index if collided
int collidesWithFood(bool headOnly) {
	struct Segment *seg = headOnly ? &snake : lastSegment;
	do {
		for (int i = 0; i < MAX_FOOD; i++) {
			if ((seg->x == food[i].x) && (seg->y == food[i].y)) {
				return i;
			}
		}

		seg = seg->prev;
	} while (!headOnly && (seg->prev != lastSegment));

	return 0;
}

void checkForFood() {
	int foodIndex = collidesWithFood(true);

	if (foodIndex) {
		addSegment();

		do {
			food[foodIndex].x = ((randint() % MAX_X) + 1) * TILE_SIZE;
			food[foodIndex].y = ((randint() % MAX_Y) + 1) * TILE_SIZE;
		} while (collidesWithFood(false));
	}
}

int main() {
	lastTick = RTC_GetTicks() - TICK_RATE * 2;

	MAX_X = LCD_WIDTH_PX / TILE_SIZE - 1;
	MAX_Y = LCD_HEIGHT_PX / TILE_SIZE - 1;

	while (true) {
		if ((RTC_GetTicks() - lastTick) > TICK_RATE) {
			Bdisp_AllClr_VRAM();

			if (state == STATE_MENU) {
				PrintXY(1, 1, "--Snake:", TEXT_MODE_NORMAL, COLOR_BLACK);
				PrintXY(1, 2, "--Definitive Edition", TEXT_MODE_NORMAL, COLOR_BLACK);
				PrintXY(1, 3, "--Exe to start", TEXT_MODE_NORMAL, COLOR_BLACK);
			} else if (state == STATE_GAME) {
				lastSegment->x = snake.x;
				lastSegment->y = snake.y;

				lastSegment = lastSegment->prev;

				if (direction % 2 == 0) {
					snake.x -= (direction - 1) * TILE_SIZE;

					if (snake.x > (MAX_X - 1) * TILE_SIZE) {
						snake.x = TILE_SIZE;
					} else if (snake.x < TILE_SIZE) {
						snake.x = MAX_X * TILE_SIZE;
					}
				} else {
					snake.y -= (direction - 2) * TILE_SIZE;

					if (snake.y > MAX_Y * TILE_SIZE) {
						snake.y = TILE_SIZE;
					} else if (snake.y < TILE_SIZE) {
						snake.y = MAX_Y * TILE_SIZE;
					}
				}

				checkForFood();
				render();
			}

			Bdisp_PutDisp_DD();

			lastTick = RTC_GetTicks();
		}

		//Should exit?
		if (input() || (state == STATE_GAME && collidesWithSelf())) {
			if (state == STATE_MENU) {
				if ((RTC_GetTicks() - lastTick) > TICK_RATE * 3) {
					break;
				}
			} else {
				Bdisp_AllClr_VRAM();

				state = STATE_MENU;

				for (struct Segment* i = lastSegment; i->prev != lastSegment; i = i->prev) {
					//TODO: does this work?
					sys_free(i);
				}

				sys_free(food);
			}
		}
	}

  return 0;
}

