#include <stdio.h>

int main()
{
    int fd = fopen("/home/users/level03/.pass", "r");
    if (fd == 0)
    {
        fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
        exit(1);
    }

    char buf[41];
    int len = fread(buf, 1, 41, fd);
    strcspn(buf, "\n");

    if (len != 41)
    {
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        exit(1);
    }

    fclose(fd);

    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");

    char username[100];
    printf("--[ Username: ");
    fgets(username, 100, stdin);
    strcspn(username, "\n");

    char password[100];
    printf("--[ Password: ");
    fgets(password, 100, stdin);
    strcspn(password, "\n");

    puts("*****************************************");

    if (strncmp(password, buf, 41) == 0)
    {
        printf("Greetings, %s!\n", username);
        system("/bin/sh");
        return 0;
    }

    printf(username);
    puts(" does not have access!");
    exit(1);
}
