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

	/*openDataFile();

	int fileSize = Bfile_GetFileSize_OS(dataFile);
	char* scoreBuff = (char*)sys_malloc(fileSize);

	printi("seek:",Bfile_SeekFile_OS(hFile, 6));
	printi("write:",Bfile_WriteFile_OS(hFile, "World!", 7));

	Bfile_ReadFile_OS(dataFile, scoreBuff, fileSize, 0);
	highScore = atoi(scoreBuff);
	sys_free(scoreBuff);*/

