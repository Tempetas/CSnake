#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/heap.h>
#include <fxcg/rtc.h>
#include <fxcg/system.h>

#include <stdbool.h>

#include "util.h"
#include "sprites.h"

//Size of individual sprites and snake tiles
#define TILE_SIZE 16

#define STATE_MENU 0
#define STATE_GAME 1

//How many apples should spawn
#define MAX_FOOD 3

//Game speed
int TICK_RATE = 15;

//Snakes head
struct Segment snake;
//Ptr to snakes tail
struct Segment *tail;
//Ptr to the previously moved segment
struct Segment *lastMovedSegment;

//Snakes moving direction
int direction = 1;
int newDirection = -1;

//Array of food locations
struct Segment *food;

int score = 0;
int highScore = 0;

int state = STATE_MENU;
int lastTick;

bool paused = false;

int MAX_X;
int MAX_Y;

int MAX_SCORE;

void drawSprite(struct Segment* pos, int size, float scale, color_t *sprite) {
	const int halfSize = size / 2;
	const int startX = pos->x - halfSize;
	const int startY = pos->y - halfSize;

	for (int x = startX; x < pos->x + halfSize; x++) {
		for (int y = startY; y < pos->y + halfSize; y++) {
			Bdisp_SetPoint_VRAM(x, y, sprite[(int)((x - startX) / scale) + ((int)((y - startY) / scale) * (int)(size / scale))]);
		}
	}
}

void drawHud() {
	int textX = TILE_SIZE;
	int textY = (MAX_Y + 0.65) * TILE_SIZE;

	char scoreString[15] = "Score: ";
	PrintMini(&textX, &textY, itoa(score, scoreString, 7), 0x40, 0xFFFFFFFF, 0, 0, (score == highScore) ? COLOR_GREEN : COLOR_BLACK, COLOR_WHITE, 1, 0);

	textX += TILE_SIZE * 13;

	char pstr[8] = "Paused  ";
	PrintMini(&textX, &textY, pstr, 0x40, 0xFFFFFFFF, 0, 0, (paused) ? COLOR_BLACK : COLOR_WHITE, COLOR_WHITE, 1, 0);
}

//Returns food index if collided
int collidesWithFood(bool headOnly) {
	struct Segment *seg = headOnly ? &snake : tail;
	do {
		for (int i = 0; i < MAX_FOOD; i++) {
			if (seg->x == food[i].x && seg->y == food[i].y) {
				return i;
			}
		}

		seg = seg->prev;
	} while (!headOnly && (seg->prev != tail));

	return -1;
}

void randomizeFood(int foodIndex) {
	bool collidesWithOther;

	do {
		collidesWithOther = false;

		food[foodIndex].x = ((randint() % (MAX_X - 1)) + 1) * TILE_SIZE;
		food[foodIndex].y = ((((int)(randint() * 0.75) + randint()) % MAX_Y) + 1) * TILE_SIZE;

		for (int i = 0; i < MAX_FOOD; i++) {
			if (i != foodIndex && (food[i].x == food[foodIndex].x && food[i].y == food[foodIndex].y)) {
				collidesWithOther = true;
				break;
			}
		}

		//Code repetition lvl. 100
		if (snake.x == food[foodIndex].x && snake.y == food[foodIndex].y) {
				collidesWithOther = true;
				continue;
		}

		struct Segment *seg = tail;
		do {
			if (seg->x == food[foodIndex].x && seg->y == food[foodIndex].y) {
				collidesWithOther = true;
				break;
			}

			seg = seg->prev;
		} while (seg != tail);

	} while (collidesWithOther);
}

void addSegment() {
	struct Segment* seg = sys_malloc(sizeof(struct Segment));

	seg->x = tail->x;
	seg->y = tail->y;

	seg->prev = tail;
	tail = seg;

	lastMovedSegment->prev = seg;

	score++;

	if (score > highScore) {
		highScore = score;
	}

	drawHud();
}

void drawRect(const int posX, const int posY, const int sizeX, const int sizeY, const color_t color) {
	for (int x = posX; x < posX + sizeX; x++) {
		for (int y = posY; y < posY + sizeY; y++) {
			Bdisp_SetPoint_VRAM(x, y, color);
		}
	}
}

void initDisplay() {
	//Hide the status area
	EnableStatusArea(3);

	//Enable fancy colors
	Bdisp_EnableColor(1);
}

void drawMenu() {
	PrintXY(1, 1, "--Snake:", TEXT_MODE_NORMAL, COLOR_BLACK);
	PrintXY(1, 2, "--Definitive Edition", TEXT_MODE_NORMAL, COLOR_BLACK);

	char scoreStr[25] = "--Highscore: ";
	PrintXY(1, 4, itoa(highScore, scoreStr, 13), TEXT_MODE_NORMAL, COLOR_BLACK);

	PrintXY(1, 6, "--F1 to start", TEXT_MODE_NORMAL, COLOR_BLACK);
}

void drawBorders() {
	const color_t borderColor = COLOR_GREEN;
	const int borderWidth = 2;

	drawRect(TILE_SIZE / 2 - borderWidth, TILE_SIZE / 2 - borderWidth, (MAX_X - 1) * TILE_SIZE + borderWidth, borderWidth, borderColor);
	drawRect(TILE_SIZE / 2 - borderWidth, ((MAX_Y + 0.5) * TILE_SIZE), (MAX_X - 1) * TILE_SIZE + borderWidth, borderWidth, borderColor);

	drawRect(TILE_SIZE / 2 - borderWidth, TILE_SIZE / 2, borderWidth, MAX_Y * TILE_SIZE, borderColor);
	drawRect((MAX_X - 0.5) * TILE_SIZE, TILE_SIZE / 2 - borderWidth, borderWidth, MAX_Y * TILE_SIZE + borderWidth * 2, borderColor);
}

bool input() {
	short unsigned key;
	int column, row;

	int keyPressType = GetKeyWait_OS(&column, &row, KEYWAIT_HALTOFF_TIMEROFF, 0, 0, &key);

	if (keyPressType == KEYREP_KEYEVENT) {
		if ((column == 0x07) && (row == 0x0A)) {
			if (state != STATE_GAME) {
				state = STATE_GAME;

				initDisplay();

				Bdisp_AllClr_VRAM();

				drawBorders();

				lastRand = RTC_GetTicks();

				int startPos = TILE_SIZE * 4;

				snake.x = startPos;
				snake.y = startPos;

				tail = sys_malloc(sizeof(struct Segment));
				tail->prev = tail;
				lastMovedSegment = tail;

				score = 0;

				drawHud();

				food = sys_malloc(sizeof(struct Segment) * MAX_FOOD);

				for (int i = 0; i < MAX_FOOD; i++) {
					randomizeFood(i);
					drawSprite(&food[i], TILE_SIZE, 1, SPRITE_APPLE);
				}
			}
		} else if (state == STATE_GAME) {
			//Arrow keys
			const int hash = row * 0xFF + column;

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

			if (column == 0x07) {
				if (row >= 0x08 && row <= 0x09) {
					paused = (row == 0x09);

					if (!paused) {
						//TODO: this looks messy
						drawBorders();
					}

					drawHud();
				}
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
	for (struct Segment *seg = tail; seg->prev != tail; seg = seg->prev) {
			if ((snake.x == seg->x) && (snake.y == seg->y)) {
			return true;
		}
	}

	return false;
}

void checkForFood() {
	const int foodIndex = collidesWithFood(true);

	if (foodIndex != -1) {
		addSegment();

		randomizeFood(foodIndex);

		drawSprite(&food[foodIndex], TILE_SIZE, 1, SPRITE_APPLE);
	}
}

int main() {
	lastTick = RTC_GetTicks() - (TICK_RATE + 1);

	MAX_X = LCD_WIDTH_PX / TILE_SIZE;
	MAX_Y = LCD_HEIGHT_PX / TILE_SIZE - 1;

	MAX_SCORE = (MAX_X * MAX_Y) - MAX_FOOD - 13;

	openDataFile();
	loadHighscore(&highScore);
	closeDataFile();

	initDisplay();
	Bdisp_AllClr_VRAM();

	drawMenu();

	while (true) {
		if ((RTC_GetTicks() - lastTick) > TICK_RATE) {
			if (state == STATE_MENU) {
				if (input()) {
					//Hack for quitting
					int key;
					GetKey(&key);
				}
			} else if (state == STATE_GAME && !paused) {
				const int halfSize = TILE_SIZE / 2;

				struct display_fill segArea = {.x1 = tail->x - halfSize, .x2 = tail->x + halfSize - 1, .y1 = tail->y - halfSize, .y2 = tail->y + halfSize - 1, .mode = 0};

				Bdisp_AreaClr(&segArea, 1, COLOR_WHITE);

				tail->x = snake.x;
				tail->y = snake.y;

				lastMovedSegment = tail;

				tail = tail->prev;

				drawSprite(&snake, TILE_SIZE, 2, SPRITE_SEGMENT);

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

				drawSprite(&snake, TILE_SIZE, 2, SPRITE_HEAD);
				drawSprite(tail, TILE_SIZE, 2, SPRITE_SEGMENT);

				checkForFood();
			} else if (paused) {
				//TODO:
				//Allow users to go to the main menu when paused
				/*int key;
				GetKey(&key);*/
			}

			Bdisp_PutDisp_DD();

			lastTick = RTC_GetTicks();
		}

		if (state == STATE_GAME) {
			bool collides = collidesWithSelf();
			if (collides || input() || score >= MAX_SCORE) {
				state = STATE_MENU;

				openDataFile();
				saveHighscore(&highScore);
				closeDataFile();

				if (collides) {
					Bdisp_AllClr_VRAM();

					//So the player has time to realize they died
					struct Segment pos = {.x = MAX_X / 2 * TILE_SIZE, .y = (MAX_Y + 1) / 2 * TILE_SIZE};
					drawSprite(&pos, 64, 4, SPRITE_SKULL);

					Bdisp_PutDisp_DD();

					OS_InnerWait_ms(750);
				} else {
					//So we dont get sent to the main menu immediately
					OS_InnerWait_ms(500);
				}

				Bdisp_AllClr_VRAM();

				drawMenu();

				{
					struct Segment* i = tail;
					struct Segment* prevprev = i->prev;
					do {
						sys_free(i);
						i = prevprev;
						prevprev = i->prev;
					} while (prevprev != tail);
				}

				sys_free(food);
			}
		}
	}

  return 0;
}

