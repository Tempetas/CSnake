//Stolen from the internet

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

