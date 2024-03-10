// gcc -m32 source.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/ptrace.h>

void clear_stdin()
{
    int c = 0; // -0xc(%ebp)

    while (1)
    {
        c = getchar();
        if (c == '\n' || c == 0xff)
            break;
    }
}

unsigned int get_unum()
{
    unsigned int unum = 0; // -0xc(%ebp)

    fflush(stdout);
    scanf("%u", &unum);
    clear_stdin();
    return unum;
}

// Slightly incorrect, but whatever
void prog_timeout()
{
    exit(1); // Supposed to be a system call but cannot reproduce
}

void enable_timeout_cons()
{
    signal(SIGALRM, prog_timeout);
    alarm(60);
}
// any above functions are useless

int auth(char *login, unsigned int serial)
{
    int i; // -0x14(%ebp)
    int hash; // -0x10(%ebp)
    size_t login_len; // -0xc(%ebp)

    login[strcspn(login, "\n")] = 0;
    login_len = strnlen(login, 32);

    if (login_len <= 5)
        return 1;

    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
    {
        puts("\x1B[32m.---------------------------.");
        puts("\x1B[31m| !! TAMPERING DETECTED !!  |");
        puts("\x1B[32m'---------------------------'");
        return 1;
    }

    hash = (login[3] ^ 4919) + 6221293;
    for (i = 0; i < login_len; i++)
    {
        if (login[i] <= 31)
            return 1;
        hash += (hash ^ (unsigned int)login[i]) % 0x539;
    }

    if (serial != hash)
        return 1;
    return 0;
}

// slight differences in assembly for stack variables (0x10 more :D)
int main()
{
    unsigned int serial; // 0x18(%esp)
    char login[32]; // 0x1c(%esp)

    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");

    printf("-> Enter Login: ");
    fgets(login, 32, stdin);

    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");

    printf("-> Enter Serial: ");
    scanf("%u", &serial);

    if (auth(login, serial))
    {
        puts("Authenticated!");
        system("/bin/sh");
        return 0;
    }
    else
        return 1;
}
