#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define UINT unsigned int
#define Byte unsigned char

void print_bin(char *str, UINT data) { // 데이터를 이진수 문자열로 출력한다.
    int i;

    printf("%s", str);
    for(i=sizeof(UINT)*8-1; i>=0; i--) {
        if(data & ((int)pow(2, i)))
            printf("%d", 1);
        else
            printf("%d", 0);
        if(i%4==0)
            printf(" ");
    }
    printf("\n");
}

char mapping(Byte value) {
    if(value < 26) {
        return 'A' + value;
    }
    else if(value >= 52) {
        if(value == 62)
            return '+';
        else if(value == 63)
            return '/';
        return '0' + (value - 52);
    }
    else {
        return 'a' + (value - 26);
    }
}

void trans(UINT block, char ascii[5], int padding) {
    UINT getmask = 0x00FC0000;

    ascii[0] = mapping((block & getmask) >> 18);
    ascii[1] = mapping((block & (getmask >> 6)) >> 12);
    if(padding < -1) {
        ascii[2] = '=';
    }
    else {
        ascii[2] = mapping((block & (getmask >> 12)) >> 6);
    }
    if(padding < 0) {
        ascii[3] = '=';
    }
    else {
        ascii[3] = mapping(block & (getmask >> 18));
    }
    ascii[4] = 0;
}

void encoding(char string[]) {
    int i=0;
    int buff_size;
    int str_size;
    UINT block;
    char *buff;

    str_size = strlen(string);
    buff_size = ceil(sizeof(char) * (str_size * 1.2));
    buff = (char*)malloc(buff_size);
    memset(buff, 0, buff_size);

    while(i<str_size) {
        block = 0;
        block = string[i++];
        block = block << 8;
        block = block | string[i++];
        block = block << 8;
        block = block | string[i++];
        trans(block, buff+((i-3)/3*4), str_size-i);
    }

    printf("%d\n", strlen(buff));
    printf("%s\n", buff);
    for(i=0; i<strlen(buff); i++) {
        printf("%02x ", buff[i]);
        if(i%4 == 3) {
            printf(" ");
        }
        if(i%16 == 15) {
            printf("\n");
        }
    }
    free(buff);
}

int main(int argc, char *argv[]) {
    char *text = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    encoding(argv[1]);

    return 0;
}
