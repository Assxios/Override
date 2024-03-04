// gcc -m32 source.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    exit(1); // Supposed to be a system call but cannot reproduce
}

int read_number(int *buffer)
{
    unsigned int var = 0; // -0xc(%ebp)

    printf(" Index: ");
    var = get_unum();
    printf(" Number at data[%u] is %u\n",var, buffer[var]);
    return 0;
}

int store_number(int *buffer)
{
    unsigned int nbr = 0; // -0x10(%ebp)
    unsigned int idx = 0; // -0xc(%ebp)

    printf(" Number: ");
    nbr = get_unum();
    printf(" Index: ");
    idx = get_unum();
    if ((idx % 3 == 0) || (idx >> 0x18 == 0xb7)) // 0x18 is 24 and 0xb7 is 183
    {
        puts(" *** ERROR! ***");
        puts("   This index is reserved for wil!");
        puts(" *** ERROR! ***");
        return 1;
    }
    else
    {
        buffer[idx] = nbr;
        return 0;
    }
    return 0;
}

int main(int argc, char **argv, char **env)
{
    int ret = 0; // 0x1b4(%esp)
    char cmd[20] = {0}; // 0x1b8(%esp) - 0x1c8(%esp)
    unsigned int buff[100] = {0}; // 0x24(%esp)

    while (*argv)
    {
        memset(*argv, 0, strlen(*argv));
        *argv++;
    }
    while (*env)
    {
        memset(*env, 0, strlen(*env));
        *env++;
    }

    puts("----------------------------------------------------\n  Welcome to wil's crappy number storage service!   \n----------------------------------------------------\n Commands:                                          \n    store - store a number into the data storage    \n    read  - read a number from the data storage     \n    quit  - exit the program                        \n----------------------------------------------------\n   wil has reserved some storage :>                 \n----------------------------------");
    while (1)
    {
        printf("Input command");
        ret = 1;
        fgets(cmd, 20, stdin);
        cmd[strlen(cmd) - 1] = 0;

        if (strncmp(cmd, "store", 5) == 0)
            ret = store_number(buff);
        else if (strncmp(cmd, "read", 4) == 0)
            ret = read_number(buff);
        else if (strncmp(cmd, "quit", 4) == 0)
            break;

        if (ret)
            printf(" Completed %s command successfully\n", cmd);
        else
            printf(" Failed to do %s command\n", cmd);
        memset(cmd, 0, 20);
    }
    return 0;
}