// gcc -m32
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void clear_stdin()
{
    unsigned char var = 0; // -0x9(%ebp)
    while (1)
    {
        var = getchar();
        if (var == '\n' || var == 0xff)
            break;
    }
}

unsigned int get_unum()
{
    unsigned int var1 = 0; // -0xc(%ebp)

    fflush(stdout);
    scanf("%u", &var1);
    clear_stdin();
    return var1;
}

// Slightly incorrect, but whatever
void prog_timeout()
{
    exit(1); // Supposed to be a system call but cannot reproduce
}
// any above functions are useless

// Missing optimization in assembly
void decrypt(unsigned int key)
{
    unsigned char buf[] = "Q}|u`sfg~sf{}|a3"; // -0x1d(%ebp) - -0xd(%ebp)
    // unsigned char buf[] = {0x75, 0x7c, 0x7d, 0x51, 0x67, 0x66, 0x73, 0x60, 0x7b, 0x66, 0x73, 0x7e, 0x33, 0x61, 0x7c, 0x7d, 0x00};
    unsigned int len = strlen((char*)buf); // -0x24(%ebp)
    unsigned int i = 0; // -0x28(%ebp)

    for (i; i < len; i++)
        buf[i] ^= key; // XOR decryption

    if (strncmp((char*)buf, "Congratulations!", 0x11) == 0)
        system("/bin/sh");
    else
        puts("\nInvalid Password");
}

void test(unsigned int usr_value, unsigned int pass)
{
    unsigned int var = pass - usr_value; // -0xc(%ebp)

    switch (var)
    {
        case 0x1:
            decrypt(var);
            break;
        case 0x2:
            decrypt(var);
            break;
        case 0x3:
            decrypt(var);
            break;
        case 0x4:
            decrypt(var);
            break;
        case 0x5:
            decrypt(var);
            break;
        case 0x6:
            decrypt(var);
            break;
        case 0x7:
            decrypt(var);
            break;
        case 0x8:
            decrypt(var);
            break;
        case 0x9:
            decrypt(var);
            break;
        case 0x10:
            decrypt(var);
            break;
        case 0x11:
            decrypt(var);
            break;
        case 0x12:
            decrypt(var);
            break;
        case 0x13:
            decrypt(var);
            break;
        case 0x14:
            decrypt(var);
            break;
        case 0x15:
            decrypt(var);
            break;
        default:
            decrypt(rand());
            break;
    }
}

// Missing optimization in assembly
int main()
{
    int var; // 0x1c(%esp)

    srand(time(0));
    puts("***********************************");
    puts("*\t\tlevel03\t\t**");
    puts("***********************************");

    printf("Password:");
    scanf("%d", &var);
    test(var, 322424845); // 0x1337d00d

    return 0;
}