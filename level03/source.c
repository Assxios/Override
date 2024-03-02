#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

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

void prog_timeout()
{
    _exit(1); // Slightly incorrect, but whatever
}
// any above functions are useless

void decrypt(unsigned int key)
{
    unsigned char data[] = {0x75, 0x7c, 0x7d, 0x51, 0x67, 0x66, 0x73, 0x60, 0x7b, 0x66, 0x73, 0x7e, 0x33, 0x61, 0x7c, 0x7d, 0x00}; // -0x1d(%ebp) - -0x11(%ebp)
    // Q}|u`sfg~sf{}|a3
    int data_length = strlen((char*)data); // -0xc(%ebp)
    int i = 0; // -0xd(%ebp) 

    for (i = 0; i < data_length; i++) {
        data[i] ^= key; // XOR decryption
    }

    if (strncmp((char*)data, "Congratulations!", 0x11) == 0) {
        system("/bin/sh");
    } else {
        puts("\nInvalid Password");
    }
}

void test(unsigned int usr_value, unsigned int pass)
{
    unsigned int var = pass - usr_value; // -0xc(%ebp)

    switch (var)
    {
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
            printf("Good... Wait what?\n"); // REMOVE
            decrypt(var);
            break;
        default:
            printf("Nope %d\n", var); // REMOVE
            decrypt(rand());
            break;
    }
}

int main()
{
    // xor on eax here
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