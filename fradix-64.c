//
// Title   : radix64 file mode
// Auth    : 전자상거래보안 6조
// Date    : 2017-04-22
// Version : 1.0
// Description : radix-64(base64) 인코딩, 디코딩 프로그램 파일 버전                                         
//

// Header
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// if macro
#define ENCODING 1
#define DECODING 1

// data type macro
#define UINT unsigned int
#define Byte unsigned char


void print_bin(char *str, UINT data) { // data를 이진수 문자열로 출력한다.
    int i;

    printf("%s", str);
    for(i=sizeof(UINT)*8-1; i>=0; i--) { // data를 이진 문자로 출력한다.
        if(data & ((int)pow(2, i)))
            printf("%d", 1);
        else
            printf("%d", 0);

        if(i%4==0)
            printf(" ");
    } // End for
    printf("\n");
} // End print_bin


char map_enc(Byte value) { // 인코딩용 맵핑
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
}// End map_enc

Byte map_dec(char value) { // 디코딩용 맵핑
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
} // End map_dec


void b3tob4(UINT block, char ascii[5], int size) { // 3 byte 이진 데이터를 4바이트 아스키 문자로 변환
    UINT getmask = 0x00FC0000;

    ascii[0] = map_enc((block & getmask) >> 18);
    ascii[1] = map_enc((block & (getmask >> 6)) >> 12);
    ascii[2] = size < 2 ? '=' : map_enc((block & (getmask >> 12)) >> 6);
    ascii[3] = size < 3 ? '=' : map_enc(block & (getmask >> 18));
    ascii[4] = 0;
} // End b3tob4

void b4tob3(UINT block, Byte binary[4]) { // 4 byte 아스키 문자를 3 byte 이진 데이터로 변환
    UINT getmask = 0x00FF0000;

    binary[0] = (block & getmask) >> 16;
    binary[1] = (block & (getmask >> 8)) >> 8;
    binary[2] = block & (getmask >> 16);
    binary[3] = 0;
}


void f_r64_encode(FILE *file, FILE *output) { // 파일 기술자를 읽어서 이진 데이터를 radix64로 변환
    int size;
    char o_block[5] ={0,};
    UINT block = 0;

    while(size = fread(&block, 1, 3, file)) {
        block = (block & 0x000000FF) << 16 | (block & 0x0000FF00) | (block & 0x00FF0000) >> 16;
        b3tob4(block, o_block, size);
        fputs(o_block, output);
        block = 0;
    } // End while
} // End f_r64_encode

void f_r64_decode(FILE *file, FILE *output) { // 파일 기술자를 읽어서 radix64 파일을 이진 파일로 변환
    char i_block[5] = {0,};
    Byte o_block[4] = {0,};
    UINT block = 0;

    while(fread(i_block, 1, 4, file)) {
        block = 0;
        block = map_dec(i_block[0]);
        block = block << 6;
        block = block | map_dec(i_block[1]);
        block = block << 6;
        if(i_block[2] != '=')
            block = block | map_dec(i_block[2]);
        block = block << 6;
        if(i_block[3] != '=')
            block = block | map_dec(i_block[3]);
        b4tob3(block, o_block);
        fwrite(o_block, 1, 3, output);
    } // End while
} // End f_r64_decode

int main(int argc, char *argv[]) {
    // 변수 선언
    FILE *file, *encode, *decode;

#if ENCODING
    file = fopen(argv[1], "rb");
    encode = fopen("encode.txt", "w");
    printf("encoding... ");
    f_r64_encode(file, encode);
    printf("complate!\n");
    fclose(file);
    fclose(encode);
#endif

#if DECODING
    encode = fopen("encode.txt", "r");
    decode = fopen("decode.bin", "wb");
    printf("decoding... ");
    f_r64_decode(encode, decode);
    printf("complate!");
    fclose(encode);
    fclose(decode);
#endif

    return 0;
}
