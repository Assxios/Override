In the elf file we can see that it take an impute and compare it with 0x149c (5276 in decimal). And later a system is call

```asm
   0x080484de <+74>:    call   0x80483d0 <__isoc99_scanf@plt>
   0x080484e3 <+79>:    mov    0x1c(%esp),%eax
   0x080484e7 <+83>:    cmp    $0x149c,%eax
   0x080484ec <+88>:    jne    0x804850d <main+121>
   [...]
   0x08048501 <+109>:   call   0x80483a0 <system@plt>
```

Let test the number :)

```bash
$ ./level00
***********************************
*            -Level00 -           *
***********************************
Password:5276

Authenticated!
$ whoami
level01
```

It work, simple as that.