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