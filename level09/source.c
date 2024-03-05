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

}

void set_username(t_message *message)
{

}

// slight differences in assembly
void handle_msg()
{
    t_message msg; // -0xc0(%rbp)
    memset(msg.user, 0, 40); // -0xc0(%rbp) + 0x8c
    msg.len = 140; // -0xc(%rbp)

    set_username(&msg);
    set_msg(&msg);

    // lea instead
    puts(">: Msg sent!");
}

// slight differences in assembly
int main()
{
    // lea instead
    puts("--------------------------------------------\n|   ~Welcome to l33t-m$n ~    v1337        |\n--------------------------------------------");
    // extra eax to 0
    handle_msg();
    return 0;
}