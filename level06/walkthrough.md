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
