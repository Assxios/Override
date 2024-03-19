# Level06

## Answer
Our C source code generates almost the same assembly code as the original binary. Compile it as follows:
> The differences are purely optimizations and do not change the behavior of the program.
```
gcc -m32 source.c
```

Let's take a look at the source code:
```c
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

/*
    Missing optimizations in assembly:
    0x0804877d <+53>:    push   %eax
    0x0804877e <+54>:    xor    %eax,%eax
    0x08048780 <+56>:    je     0x8048785 <auth+61>
    0x08048782 <+58>:    add    $0x4,%esp
    0x08048785 <+61>:    pop    %eax

    It doesnt change the behavior at all and idk how to reproduce it
*/
int auth(char *login, unsigned int serial)
{
    int i; // -0x14(%ebp)
    int hash; // -0x10(%ebp)
    int login_len; // -0xc(%ebp)

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

/*
    Missing optimizations in assembly:
    0x08048895 <+28>:    push   %eax
    0x08048896 <+29>:    xor    %eax,%eax
    0x08048898 <+31>:    je     0x804889d <main+36>
    0x0804889a <+33>:    add    $0x4,%esp
    0x0804889d <+36>:    pop    %eax

    Aswell as multiple slight differences with different registers for optimization
    It doesnt change the behavior at all and idk how to reproduce it
*/
int main()
{
    int padding[4]; // 0x18(%esp)
    unsigned int serial; // 0x28(%esp)
    char login[32]; // 0x2c(%esp)

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

    if (!auth(login, serial))
    {
        puts("Authenticated!");
        system("/bin/sh");
        return 0;
    }
    else
        return 1;
}
```

In the source code, there isn't any obvious vulnerabilities, instead we have to find the correct input for the `auth` function to return `0`. We can see that the `auth` function hashes the `login` input and compares it to the `serial` input. If the `login` is longer than 5 characters and the hash is equal to the `serial`, the program will execute `/bin/sh`.

We'll use `gdb` to find the result of the hash system:
```bash
(gdb) b *auth+114 # Breaking before ptrace
Breakpoint 1 at 0x80487ba

(gdb) b *auth+283 # Breaking after hash loop
Breakpoint 2 at 0x8048863

(gdb) r # Run the program
Starting program: /home/users/level06/level06
***********************************
*               level06           *
***********************************
-> Enter Login: assxios
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: assxios

Breakpoint 1, 0x080487ba in auth ()
(gdb) set $eax=0 # Bypass ptrace
(gdb) c
Continuing.

Breakpoint 2, 0x08048863 in auth ()
(gdb) x/d $ebp-0x10 # Print the result of the hash
0xffffd698:     6233782
```

Let's try to log in with the serial we found:
```bash
level06@OverRide:~$ ./level06
***********************************
*               level06           *
***********************************
-> Enter Login: assxios
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6233782
Authenticated!
$ whoami
level07
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```

We are now level07! Let's move on to the next level.