#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define UINT unsigned int
#define Byte unsigned char

char mapping324(Byte value) {
    if (value < 26)
        return 'A' + value;
    else if (value < 52)
        return 'a' + (value - 26);
    else if (value < 62)
        return '0' + (value - 52);
    else if (value == 62)
        return '+';
    else
        return '/';
}

Byte mapping423(char value) {
    if (value < 'a')
        return value - 'A';
    else if (value < '0')
        return (value + 26) - 'a';
    else if (value < '+')
        return (value + 52) - '0';
    else if (value == '+')
        return 62;
    else
        return 63;
}

void b3tob4(UINT block, char ascii[5], int padding) {
    UINT getmask = 0x00FC0000;

    ascii[0] = mapping324((block & getmask) >> 18);
    ascii[1] = mapping324((block & (getmask >> 6)) >> 12);
    if(padding < -1)
        ascii[2] = '=';
    else
        ascii[2] = mapping324((block & (getmask >> 12)) >> 6);
    if(padding < 0)
        ascii[3] = '=';
    else
        ascii[3] = mapping324(block & (getmask >> 18));
    ascii[4] = 0;
}

void b4tob3(UINT block, Byte ascii[4], int padding) {
    UINT getmask = 0x00FF0000;

    ascii[0] = mapping423((block & getmask) >> 16);
    ascii[1] = mapping423((block & (getmask >> 8)) >> 8);
    ascii[2] = mapping423((block & (getmask >> 16)));
    ascii[3] = 0;
}

void encoding(char str[]) {
    int i=0;
    int buff_size;
    int str_size;
    UINT block;
    char *buff;

    str_size = strlen(str);
    buff_size = ceil(sizeof(char) * (str_size * 1.2));
    buff = (char*)malloc(buff_size);
    memset(buff, 0, buff_size);

    while(i<str_size) {
        block = 0;
        block = str[i++];
        block = block << 8;
        block = block | str[i++];
        block = block << 8;
        block = block | str[i++];
        b3tob4(block, buff+((i-3)/3*4), str_size-i);
    }

    printf("%s\n", buff);
    free(buff);
}

void decoding(char str[]) {
    int i=0;
    int buff_size;
    int str_size;
    UINT block;
    char *buff;

    str_size = strlen(str);
    buff_size = ceil(sizeof(char) * (str_size * 1.2));
    buff = (char*)malloc(buff_size);
    memset(buff, 0, buff_size);

    while(i<str_size) {
        block = 0;
        block = str[i++];
        block = block << 6;
        block = block | str[i++];
        block = block << 6;
        block = block | str[i++];
        block = block << 6;
        block = block | str[i++];
        b3tob4(block, buff+((i-3)/3*4));
    }

    printf("%s\n", buff);
    free(buff);
}

int main(int argc, char *argv[]) {
    char *text = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    encoding(argv[1]);

    return 0;
}
