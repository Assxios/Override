#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ptrace.h>

void clear_stdin()
{
    int c = 0;

    while (1)
    {
        c = getchar();
        if (c == '\n' || c == 0xff)
            break;
    }
}

unsigned int get_unum()
{
    unsigned int c = 0;

    fflush(stdout);
    scanf("%u", &c);
    clear_stdin();
    return c;
}

void prog_timeout()
{
    exit(1);
}

void enable_timeout_cons()
{
    signal(SIGALRM, prog_timeout);
    alarm(60);
}

int	auth(char *login, int serial)
{
	login[strcspn(login, "\n")] = 0;

	size_t len = strnlen(login, 32);
	if (len <= 5)
		return 1;

	if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
	{
		puts("\033[32m.---------------------------.");
		puts("\033[31m| !! TAMPERING DETECTED !!  |");
		puts("\033[32m'---------------------------'");
		return 1;
	}

	unsigned int key = (login[3] ^ 4919) + 6221293;
	for (size_t i = 0; i < len; i++)
	{
		if (login[i] <= 31)
			return 1;
		key += (login[i] ^ key) * 1337;
	}

	return serial != key;
}

int	main()
{
	puts("***********************************");
	puts("*\t\tlevel06\t\t**");
	puts("***********************************");

	char login[32];
	printf("-> Enter Login: ");
	fgets(login, 32, stdin);

	puts("***********************************");
	puts("***** NEW ACCOUNT DETECTED ********");
	puts("***********************************");

	int serial;
	printf("-> Enter Serial: ");
	scanf("%u", &serial);

	if (!auth(login, serial))
		return 1;

	puts("Authenticated!");
	system("/bin/sh");
	return 0;
}
