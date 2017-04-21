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

char map_enc(Byte value) {
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
Byte map_dec(char value) {
    if (value >= 'a')
        return 26 + (value - 'a');
    else if(value >= 'A')
        return value - 'A';
    else if(value >= '0')
        return 52 + (value - '0');
    else if(value == '+')
        return 62;
    else
        return 63;
}

void b3tob4(UINT block, char ascii[5], int padding) {
    UINT getmask = 0x00FC0000;

    ascii[0] = map_enc((block & getmask) >> 18);
    ascii[1] = map_enc((block & (getmask >> 6)) >> 12);
    ascii[2] = padding < -1 ? '=' : map_enc((block & (getmask >> 12)) >> 6);
    ascii[3] = padding < 0 ? '=' : map_enc(block & (getmask >> 18));
    ascii[4] = 0;
}
void b4tob3(UINT block, Byte binary[4]) {
    UINT getmask = 0x00FF0000;

    binary[0] = (block & getmask) >> 16;
    binary[1] = (block & (getmask >> 8)) >> 8;
    binary[2] = block & (getmask >> 16);
    binary[3] = 0;
}

void encoding(Byte *str, char *output) {
    int i=0;
    int str_size;
    UINT block;

    str_size = strlen(str);

    while(i<str_size) {
        block = 0;
        block = str[i++];
        block = block << 8;
        block = block | str[i++];
        block = block << 8;
        block = block | str[i++];
        b3tob4(block, output+((i-3)/3*4), str_size-i);
    }
}
void decoding(char *str, Byte *output) {
    int i=0;
    UINT block;
    int str_size;

    str_size = strlen(str);

    while(i<str_size) {
        block = 0;
        block = map_dec(str[i++]);
        block = block << 6;
        block = block | map_dec(str[i++]);
        block = block << 6;
        if(str[i] != '=') {
            block = block | map_dec(str[i++]);
        }
        else {
            i++;
        }
        block = block << 6;
        if(str[i] != '=') {
            block = block | map_dec(str[i++]);
        }
        else {
            i++;
        }
        b4tob3(block, output+((i-4)/4*3));
    }
}

int main(int argc, char *argv[]) {
    char *buff;// = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    Byte *decode;
    int str_size, buff_size;

    str_size = strlen(argv[1]);
    // buff size set
    buff_size = ceil(sizeof(char) * (str_size * 1.2));
    buff = (char*)malloc(buff_size);
    memset(buff, 0, buff_size);
    // decode size set
    decode = (Byte*)malloc(sizeof(char) * str_size);
    memset(decode, 0, str_size);


    encoding(argv[1], buff);
    printf("%s\n", buff);
    decoding(buff, decode);
    printf("%s\n", decode);

    return 0;
}
