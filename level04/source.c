#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

void clear_stdin()
{
    int c = 0;

    while (1)
    {
        c = getchar();
        if (c == '\n' || c == 0xff)
            break;
    }
}

unsigned int get_unum()
{
    unsigned int c = 0;

    fflush(stdout);
    scanf("%u", &c);
    clear_stdin();
    return c;
}

void prog_timeout()
{
    exit(1);
}

void enable_timeout_cons()
{
    signal(SIGALRM, prog_timeout);
    alarm(60);
}

int main()
{
    pid_t pid = fork();
    char buf[128];
    int wstatus;

    if (pid == 0)
    {
        prctl(1, 1);
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        puts("Give me some shellcode, k");
        gets(buf);
        return 0;
    }

    do
    {
        wait(&wstatus);
        if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus))
        {
            puts("child is exiting...");
            return 0;
        }
    }
    while (ptrace(PTRACE_PEEKUSER, pid, 44, 0) != 11);

    puts("no exec() for you");
    kill(pid, 9);
    return 0;
}
