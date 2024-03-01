// gcc -fno-stack-protector -m32 main.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char a_user_name[256];

int verify_user_name()
{
    puts("verifying username....\n");
    return strncmp(a_user_name, "dat_wil", 7);
}

int verify_user_pass(char *a_user_pass)
{
    return strncmp(a_user_pass, "admin", 5);
}

int main()
{
    char buf[64]; // 0x1c
    int ret; // 0x5c

    memset(buf, 0, 64);
    ret = 0;

    puts("********* ADMIN LOGIN PROMPT *********");

    printf("Enter Username: ");
    fgets(a_user_name, 256, stdin);
    ret = verify_user_name();
    if (ret != 0)
    {
        puts("nope, incorrect username...");
        return 1;
    }

    puts("Enter Password: ");
    fgets(buf, 100, stdin);
    ret = verify_user_pass(buf);
    if (ret == 0 || ret != 0)
    {
        puts("nope, incorrect password...");
        return 1;
    }

    return 0;
}