#include <fxcg/file.h>

struct Segment {
	int x;
	int y;

	struct Segment *prev;
};

unsigned int lastRand;

unsigned int randint() {
	lastRand = 0x41C64E6D * lastRand + 0x3039;
	return lastRand;
}

int abs(int x) {
    return x < 0 ? -x : x;
}

void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

char* reverse(char *buffer, int i, int j) {
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

char* itoa(int value, char* buffer, int offset) {
    int n = value;

    int i = offset;
    while (n) {
        int r = n % 10;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        } else {
            buffer[i++] = 48 + r;
        }

        n = n / 10;
    }

    if (i == offset) {
        buffer[i++] = '0';
    }

    if (value < 0) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    return reverse(buffer, offset, i - 1);
}

int atoi(char* str) {
    int res = 0;

    for (int i = 0; str[i] != '\0'; ++i) {
        res = res * 10 + str[i] - '0';
    }

    return res;
}

int dataFile = -1;
#define DATA_FILE_SIZE 32

void openDataFile() {
  #define CREATEMODE_FILE 1
  #define WRITE 2
  #define READWRITE 3

  #define FILE_PATH "\\\\fls0\\snake-score.sav"

  unsigned short filePath[sizeof(FILE_PATH) * 2];

  Bfile_StrToName_ncpy(filePath, (char*)FILE_PATH, sizeof(FILE_PATH));
  dataFile = Bfile_OpenFile_OS(filePath, READWRITE, 0);

  if (dataFile < 0) {
    unsigned int size = DATA_FILE_SIZE;

    if(Bfile_CreateEntry_OS(filePath, CREATEMODE_FILE, &size) >= 0) {
      dataFile = Bfile_OpenFile_OS(filePath, READWRITE, 0);
    }
  }
}

void closeDataFile() {
  Bfile_CloseFile_OS(dataFile);
}

void saveHighscore(int *highScore) {
  char tmpBuf[DATA_FILE_SIZE] = {0};
  itoa(*highScore, tmpBuf, 0);
  Bfile_SeekFile_OS(dataFile, 0);
  Bfile_WriteFile_OS(dataFile, tmpBuf, sizeof(char) * DATA_FILE_SIZE);
  tmpBuf[0] = '\0';
  Bfile_WriteFile_OS(dataFile, tmpBuf, sizeof(char));
}

void loadHighscore(int *highScore) {
  char tmpBuf[DATA_FILE_SIZE];
  Bfile_ReadFile_OS(dataFile, tmpBuf, sizeof(int), 0);
  *highScore = atoi(tmpBuf);
}

