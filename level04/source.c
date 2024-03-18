// gcc -m32 -z execstack -fno-stack-protector source.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/ptrace.h>

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

void enable_timeout_cons()
{
    signal(SIGALRM, prog_timeout);
    alarm(60);
}
// any above functions are useless

// slight differences in assembly with stack variables alignment and optimization
int main()
{
    pid_t pid = fork(); // 0xac(%esp)
    char buff[128] = { 0 };  // 0x20(%esp)

    int ptrace_data = 0; // 0xa8(%esp)
    int stat_loc = 0;    // 0x1c(%esp)
    int backup;      // 0xa4(%esp)

    if (pid == 0)
    {
        prctl(1, 1);
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        puts("Give me some shellcode, k");
        gets(buff);
    }
    else
    {
        do
        {
            wait(&stat_loc);
            backup = stat_loc;

            if (WIFEXITED(stat_loc) || WIFSIGNALED(stat_loc))
            {
                puts("child is exiting...");
                return 0;
            }

            ptrace_data = ptrace(PTRACE_PEEKUSER, pid, 44, 0);
        } while (ptrace_data != 11);

        puts("no exec() for you");
        kill(pid, 9);
    }
    return 0;
}