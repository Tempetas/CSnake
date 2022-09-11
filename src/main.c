#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/rtc.h>
#include <fxcg/file.h>

#include <stdbool.h>
#include "util.h"

//Size of individual sprites and snake tiles
#define TILE_SIZE 16

#define STATE_MENU 0
#define STATE_GAME 1

//Amount of spawning apples
#define MAX_FOOD 3

//Game speed
int TICK_RATE = 20;

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
int newDirection = -1;

//Array of food locations
struct Segment *food;

int score = 0;
int highScore = 0;

int state = STATE_MENU;
int lastTick;

int MAX_X;
int MAX_Y;

int dataFile = -1;

unsigned int lastRand;

unsigned int randint() {
	lastRand = 0x41C64E6D * lastRand + 0x3039;
	return lastRand;
}

void randomizeFood(int foodIndex) {
	bool collidesWithOther;

	do {
		collidesWithOther = false;

		food[foodIndex].x = ((randint() % (MAX_X - 1)) + 1) * TILE_SIZE;
		food[foodIndex].y = (randint() % MAX_Y) * TILE_SIZE;

		for (int i = 0; i < MAX_FOOD; i++) {
			if (i != foodIndex && (food[i].x == food[foodIndex].x && food[i].y == food[foodIndex].y)) {
				collidesWithOther = true;
				break;
			}
		}
	} while (collidesWithOther);
}

/*void openDataFile() {
	#define CREATEMODE_FILE 1
	#define WRITE 2
	#define READWRITE 3

	#define FILE_PATH "snake-score.txt"

	unsigned char pFile[sizeof(FILE_PATH) * 2] = FILE_PATH;

	int hFile;

	hFile = Bfile_OpenFile_OS(pFile, READWRITE, 0);

	if (hFile < 0) {
		int size = 64;
		Bfile_CreateEntry_OS(pFile, CREATEMODE_FILE, &size);

		hFile = Bfile_OpenFile_OS(pFile, READWRITE, 0);
	}

	dataFile = hFile;
}*/

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

void drawCircle(struct Segment* pos, int radius, color_t color) {
	const int halfSize = radius / 2;
	for (int x = pos->x - halfSize; x < pos->x + halfSize; x++) {
		for (int y = pos->y - halfSize; y < pos->y + halfSize; y++) {
			//Hack! In this house we hate math.h
			int xDiff = x - pos->x;
			int yDiff = y - pos->y;

			if (xDiff * xDiff + yDiff * yDiff < (radius * radius / 4)) {
				Bdisp_SetPoint_VRAM(x, y, color);
			}
		}
	}
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
		drawCircle(&food[i], TILE_SIZE, COLOR_RED);
	}

	/*char scoreStr[15] = "--Score: ";
	PrintXY(1, 1, itoa(MAX_X, scoreStr, 9), TEXT_MODE_NORMAL, COLOR_BLACK);*/

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
					randomizeFood(i);
				}
			}
		} else if (state == STATE_GAME) {
			//Arrow keys
			int hash = row * 0xFF + column;

			switch (hash) {
				case 0x7F8 + 0x02:
					newDirection = 0;
				break;
				case 0x7F8 + 0x03:
					newDirection = 1;
				break;
				case 0x8F7 + 0x03:
					newDirection = 2;
				break;
				case 0x8F7 + 0x02:
					newDirection = 3;
				break;
			}

			if (row == 0x0A) {
				TICK_RATE = (column >= 0x03 && column <= 0x06) ? 20 - (0x06 - column) * 5 : TICK_RATE;
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
			if (seg->x == food[i].x && seg->y == food[i].y) {
				return i;
			}
		}

		seg = seg->prev;
	} while (!headOnly && (seg->prev != lastSegment));

	return -1;
}

void checkForFood() {
	int foodIndex = collidesWithFood(true);

	if (foodIndex != -1) {
		addSegment();

		do {
			randomizeFood(foodIndex);
		} while (collidesWithFood(false) != -1);
	}
}

int main() {
	lastTick = RTC_GetTicks() - (TICK_RATE + 1);

	MAX_X = LCD_WIDTH_PX / TILE_SIZE;
	MAX_Y = LCD_HEIGHT_PX / TILE_SIZE;

	/*openDataFile();

	int fileSize = Bfile_GetFileSize_OS(dataFile);
	char* scoreBuff = (char*)sys_malloc(fileSize);

	printi("seek:",Bfile_SeekFile_OS(hFile, 6));
	printi("write:",Bfile_WriteFile_OS(hFile, "World!", 7));

	Bfile_ReadFile_OS(dataFile, scoreBuff, fileSize, 0);
	highScore = atoi(scoreBuff);
	sys_free(scoreBuff);*/

	while (true) {
		if ((RTC_GetTicks() - lastTick) > TICK_RATE) {
			Bdisp_AllClr_VRAM();

			if (state == STATE_MENU) {
				PrintXY(1, 1, "--Snake:", TEXT_MODE_NORMAL, COLOR_BLACK);
				PrintXY(1, 2, "--Definitive Edition", TEXT_MODE_NORMAL, COLOR_BLACK);
				PrintXY(1, 3, "--F1 to start", TEXT_MODE_NORMAL, COLOR_BLACK);

				char scoreStr[25] = "--High score: ";
				PrintXY(1, 4, itoa(highScore, scoreStr, 14), TEXT_MODE_NORMAL, COLOR_BLACK);

				if (input()) {
					//Hack for quitting
					int key;
					GetKey(&key);
				}
			} else if (state == STATE_GAME) {
				lastSegment->x = snake.x;
				lastSegment->y = snake.y;

				lastSegment = lastSegment->prev;

				if (newDirection != -1 && (direction - newDirection) % 2 != 0) {
					direction = newDirection;
				}

				if (direction % 2 == 0) {
					snake.x -= (direction - 1) * TILE_SIZE;

					if (snake.x > (MAX_X - 1) * TILE_SIZE) {
						snake.x = TILE_SIZE;
					} else if (snake.x < TILE_SIZE) {
						snake.x = (MAX_X - 1) * TILE_SIZE;
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

		if (state == STATE_GAME && (input() || collidesWithSelf())) {
				Bdisp_AllClr_VRAM();

				state = STATE_MENU;

				for (struct Segment* i = lastSegment; i->prev != lastSegment; i = i->prev) {
					//TODO: does this work?
					sys_free(i);
				}

				sys_free(food);
			}
	}

  return 0;
}

