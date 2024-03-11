# Level00

## Foreword
In the following walkthroughs, we will frequently reference the source code we wrote. This is due to our initial step of translating the assembly code into C code.

Our aim was to align our C code closely with the original source code, a goal we largely achieved. Nonetheless, there are some deviations, which we will highlight as needed. We will also refer to the assembly code only when necessary.

## Answer
Our C source code generates the same assembly code as the original binary. Compile it as follows:
```
gcc -m32 source.c
```

Let's take a look at the source code:
```c
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
```

The program takes an integer input with `scanf` and compares it to 5276.

If the comparison is true, the program prints `\nAuthenticated!` and executes `/bin/sh` with the same privileges as the owner's group and user.
> Because the setuid bit is set on the binary, the program will run with the privileges of the owner, which is level01. This is true for all the binaries in this project.

Otherwise, it prints `\nInvalid Password!` to stdout.

So we need to write 5276 to the program. Let's try it:
```bash
$ ./level00
***********************************
*            -Level00 -           *
***********************************
Password:5276

Authenticated!
$ whoami
level01
$ cat /home/users/level01/.pass
uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
```

We are now level01! Let's move on to the next level.