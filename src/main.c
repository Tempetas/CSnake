#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/rtc.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TILE_SIZE 16

#define TICK_RATE 20

#define STATE_MENU 0
#define STATE_GAME 1

#define MAX_FOOD 3

struct Segment {
	int x;
	int y;

	struct Segment *prev;
};

struct Segment snake;
struct Segment *food;

int snakeLength = 0;
struct Segment *lastSegment;

//right down left up
int direction = 1;

//Screw good code standarts
int state = STATE_MENU;
int lastTick;

int MAX_X, MAX_Y;

void addSegment() {
	snakeLength++;

	struct Segment* seg = sys_malloc(sizeof(struct Segment));

	seg->x = lastSegment->x;
	seg->y = lastSegment->y;

	seg->prev = lastSegment;

	//TODO: this is dumb
	struct Segment* i = lastSegment;
	for (; i->prev != lastSegment; i = i->prev);
	i->prev = seg;

	lastSegment = seg;
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

				int startPos = TILE_SIZE * 4;

				snake.x = startPos;
				snake.y = startPos;

				lastSegment = sys_malloc(sizeof(struct Segment));

				lastSegment->prev = lastSegment;

				food = sys_malloc(sizeof(struct Segment) * MAX_FOOD);

				for (int i = 0, ticks = RTC_GetTicks(); i < MAX_FOOD; i++, ticks += 23) {
					food[i].x = ((ticks % 8) + 1) * TILE_SIZE;
					food[i].y = ((ticks % 5) + 1) * TILE_SIZE;
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

			if ((column == 0x04) && (row == 0x09)) {
				Bdisp_AllClr_VRAM();

				state = STATE_MENU;

				sys_free(food);

				return true;
			}
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

		int ticks = RTC_GetTicks();

		do {
			food[foodIndex].x = ((ticks % MAX_X) + TILE_SIZE) * TILE_SIZE;
			food[foodIndex].y = ((ticks % MAX_Y) + TILE_SIZE)  * TILE_SIZE;

			//Random magic const
			ticks += 23;
		} while (collidesWithFood(false));
	}
}

int main() {
	lastTick = RTC_GetTicks() - 40;

	MAX_X = LCD_WIDTH_PX / TILE_SIZE;
	MAX_Y = LCD_HEIGHT_PX / TILE_SIZE;

	while (true) {
		if ((RTC_GetTicks() - lastTick) > 20) {
			Bdisp_AllClr_VRAM();

			DisplayStatusArea();

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

					if (snake.x > MAX_X * TILE_SIZE) {
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

		if (input() && state == STATE_MENU) {
			break;
		}
	}

  return 0;
}

