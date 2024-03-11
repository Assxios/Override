// gcc -m32 source.c
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int password; // 0x1c(%esp)

    puts("***********************************");
    puts("* \t     -Level00 -\t\t  *");
    puts("***********************************");

    printf("Password:");
    scanf("%d", &password);

    if (password == 5276)
    {
        puts("\nAuthenticated!");
        system("/bin/sh");

        return 0;
    }
    else
    {
        puts("\nInvalid Password!");

        return 1;
    }
}
