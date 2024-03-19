// gcc -fno-stack-protector -m32 -z execstack -z norelro source.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[100]; // 0x28(%esp)
    int i = 0; // 0x8c(%esp)

    fgets(buffer, 100, stdin);
    i = 0;

    while (i < strlen(buffer))
    {
        if (buffer[i] > 0x40 && buffer[i] <= 0x5a) // 0x40 is '@', 0x5a is 'Z' aka if the character is uppercase
            buffer[i] ^= 0x20; // XOR with 0x20 (32) makes it lowercase
        i++;
    }

    printf(buffer);
    exit(0);
}