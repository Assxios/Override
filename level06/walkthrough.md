the program ask for two input:

    $ ./level06
    ***********************************
    *               level06           *
    ***********************************
    -> Enter Login: whatever
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: whatever

we can see in the disassembly that the program hash the first input (login) and compare the key with the second input (serial). If the key is equal to the serial, the program will execute /bin/sh.

cause the hash function is big we dont give the asm code but you can check the c code in the source code.

more info:
- login have to be 5 char long
- ptrace is used to prevent debugging

we will use a basic input like `whatever` and use gdb to find the result of the hash system:

    (gdb) b *auth+114 //ptrace
    Breakpoint 1 at 0x80487ba
    
    (gdb) b *auth+283 //after hash loop
    Breakpoint 2 at 0x8048863
    (gdb) r
    Starting program: /home/users/level06/level06
    ***********************************
    *               level06           *
    ***********************************
    -> Enter Login: whatever
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: whatever

    Breakpoint 1, 0x080487ba in auth ()
    (gdb) set $eax=0 //pass the ptrace
    (gdb) continue
    Continuing.

    Breakpoint 2, 0x08048863 in auth ()
    (gdb) x/d $ebp-0x10 //result of the hash
    0xffffd698:     6234500

lets check this serial:

    $ ./level06
    ***********************************
    *               level06           *
    ***********************************
    -> Enter Login: whatever
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: 6234500
    Authenticated!
    $ whoami
    level07

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
    size_t login_len; // -0xc(%ebp)

    login[strcspn(login, "\n")] = 0;
    login_len = strnlen(login, 32);

    if (login_len >= 5)
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

    if (auth(login, serial))
    {
        puts("Authenticated!");
        system("/bin/sh");
        return 0;
    }
    else
        return 1;
}
```

In the source code, there isn't any obvious vulnerabilities, instead we have to find the correct input for the `test` function to not take a random key and then find the correct key between the values that will decrypt "Q}|u\`sfg~sf{}|a3" with a `XOR`.

First, we need to input one of the following values for the `test` function to not take a random key:
```
322424844 (322424845 - 1)
322424843 (322424845 - 2)
322424842 (322424845 - 3)
322424841 (322424845 - 4)
322424840 (322424845 - 5)
322424839 (322424845 - 6)
322424838 (322424845 - 7)
322424837 (322424845 - 8)
322424836 (322424845 - 9)
322424829 (322424845 - 16)
322424828 (322424845 - 17)
322424827 (322424845 - 18)
322424826 (322424845 - 19)
322424825 (322424845 - 20)
322424843 (322424845 - 21)
```

Then we need to find the correct key between these values, which after some trial and error, we find to be `18`. Therefore, the number we need to input is `322424827`.

Let's try to log in with the password we found:
```bash
$ ./level06
***********************************
*               level06           *
***********************************
-> Enter Login: whatever
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6234500
Authenticated!
$ whoami
level07
```

We are now level07! Let's move on to the next level.

