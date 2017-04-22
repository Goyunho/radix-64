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

void b3tob4(UINT block, char ascii[5], int size) {
    UINT getmask = 0x00FC0000;

    ascii[0] = map_enc((block & getmask) >> 18);
    ascii[1] = map_enc((block & (getmask >> 6)) >> 12);
    ascii[2] = size < 2 ? '=' : map_enc((block & (getmask >> 12)) >> 6);
    ascii[3] = size < 3 ? '=' : map_enc(block & (getmask >> 18));
    ascii[4] = 0;
}
void b4tob3(UINT block, Byte binary[4]) {
    UINT getmask = 0x00FF0000;

    binary[0] = (block & getmask) >> 16;
    binary[1] = (block & (getmask >> 8)) >> 8;
    binary[2] = block & (getmask >> 16);
    binary[3] = 0;
}

void f_r64_encode(FILE *file, FILE *output) {
    int size;
    char o_block[5];
    UINT block = 0;

    while(size=fread(&block, 1, 3, file)) {
        block = (block & 0x000000FF) << 16 | (block & 0x0000FF00) | (block & 0x00FF0000) >> 16;
        print_bin("block : ", block);
        b3tob4(block, o_block, size);
        fputs(o_block, output);
        block = 0;
    }
}
void f_r64_decode(char *str, Byte *output) {
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
    FILE *file, *encode, *decode;

    file = fopen(argv[1], "rb");
    encode = fopen("encode.txt", "w");
    f_r64_encode(file, encode);

    // encode = fopen("encode.txt", "r");
    // decode = fopen("decode", "wb");
    // f_r64_decode(encode, decode);

    fclose(file);
    fclose(encode);
    // fclose(decode);

    return 0;
}
