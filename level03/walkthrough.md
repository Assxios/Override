# Level03

## Answer
Our C source code generates almost the same assembly code as the original binary. Compile it as follows:
> The differences are purely optimizations and do not change the behavior of the program.
```
gcc -m32 source.c
```

Let's take a look at the source code:
```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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
// any above functions are useless

/* 
    Missing optimizations in assembly:
    0x08048693 <+51>:    push   %eax
    0x08048694 <+52>:    xor    %eax,%eax
    0x08048696 <+54>:    je     0x804869b <decrypt+59>
    0x08048698 <+56>:    add    $0x4,%esp
    0x0804869b <+59>:    pop    %eax

    And another extra use of eax.

    It doesnt change the behavior at all and idk how to reproduce it
*/
void decrypt(unsigned int key)
{
    unsigned char buf[] = "Q}|u`sfg~sf{}|a3"; // -0x1d(%ebp) - -0xd(%ebp)
    // unsigned char buf[] = {0x75, 0x7c, 0x7d, 0x51, 0x67, 0x66, 0x73, 0x60, 0x7b, 0x66, 0x73, 0x7e, 0x33, 0x61, 0x7c, 0x7d, 0x00};
    unsigned int len = strlen((char*)buf); // -0x24(%ebp)
    unsigned int i = 0; // -0x28(%ebp)

    for (i; i < len; i++)
        buf[i] ^= key; // XOR decryption

    if (strncmp((char*)buf, "Congratulations!", 0x11) == 0)
    {
        system("/bin/sh");
        return;
    }
    puts("\nInvalid Password");
}

void test(unsigned int usr_value, unsigned int pass)
{
    unsigned int var = pass - usr_value; // -0xc(%ebp)

    switch (var)
    {
        case 0x1:
            decrypt(var);
            break;
        case 0x2:
            decrypt(var);
            break;
        case 0x3:
            decrypt(var);
            break;
        case 0x4:
            decrypt(var);
            break;
        case 0x5:
            decrypt(var);
            break;
        case 0x6:
            decrypt(var);
            break;
        case 0x7:
            decrypt(var);
            break;
        case 0x8:
            decrypt(var);
            break;
        case 0x9:
            decrypt(var);
            break;
        case 0x10:
            decrypt(var);
            break;
        case 0x11:
            decrypt(var);
            break;
        case 0x12:
            decrypt(var);
            break;
        case 0x13:
            decrypt(var);
            break;
        case 0x14:
            decrypt(var);
            break;
        case 0x15:
            decrypt(var);
            break;
        default:
            decrypt(rand());
            break;
    }
}

/*
    Missing optimizations in assembly:
    0x08048863 <+9>:     push   %eax
    0x08048864 <+10>:    xor    %eax,%eax
    0x08048866 <+12>:    je     0x804886b <main+17>
    0x08048868 <+14>:    add    $0x4,%esp
    0x0804886b <+17>:    pop    %eax

    Same as above, it doesnt change the behavior at all and idk how to reproduce it
*/
int main()
{
    int var; // 0x1c(%esp)

    srand(time(0));
    puts("***********************************");
    puts("*\t\tlevel03\t\t**");
    puts("***********************************");

    printf("Password:");
    scanf("%d", &var);
    test(var, 322424845); // 0x1337d00d

    return 0;
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
level03@OverRide:~$ ./level03
***********************************
*               level03         **
***********************************
Password:322424827
$ whoami
level04
$ cat /home/users/level04/.pass
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
```

We are now level05! Let's move on to the next level.

