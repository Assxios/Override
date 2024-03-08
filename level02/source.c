// gcc -fno-stack-protector -z execstack -z norelro source.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *file; // -0x8(%rbp)
    int len; // -0xc(%rbp)
    char username[100]; // -0x70(%rbp)
    char flag[42]; // -0xa0(%rbp)
    char password[100]; // -0x110(%rbp)

    memset(username, 0, 100);
    memset(flag, 0, 42);
    memset(password, 0, 100);

    file = 0;
    len = 0;

    file = fopen("/home/users/level03/.pass", "r");
    if (file == 0)
    {
        fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
        exit(1);
    }

    len = fread(flag, 1, 41, file);
    flag[strcspn(flag, "\n")] = 0;
    if (len != 41)
    {
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        exit(1);
    }

    fclose(file);

    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");

    printf("--[ Username: ");
    fgets(username, 100, stdin);
    username[strcspn(username, "\n")] = 0;

    printf("--[ Password: ");
    fgets(password, 100, stdin);
    password[strcspn(password, "\n")] = 0;

    puts("*****************************************");

    if (strncmp(flag, password, 41) == 0)
    {
        printf("Greetings, %s!\n", username);
        system("/bin/sh");
        return 0;
    }

    printf(username);
    puts(" does not have access!");
    exit(1);
}
