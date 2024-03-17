// gcc -z execstack -z now source.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void log_wrapper(FILE *file, char *str, char *name)
{
	char buffer[264]; // -0x110(%rbp)

	strcpy(buffer, str);
	snprintf(&buffer[strlen(buffer)], 254 - strlen(buffer), name);
	buffer[strcspn(buffer, "\n")] = 0;
	fprintf(file, "LOG: %s\n", buffer);
}

int main(int argc, char **argv)
{
	FILE *log; // -0x88(%rbp)
	FILE *file; // -0x80(%rbp)
	int fd; // -0x78(%rbp)
	char c; // -0x71(%rbp)
	char dest[104]; // -0x70(%rbp)

	c = -1;
	if (argc != 2)
		printf("Usage: %s filename\n", *argv);

	log = fopen("./backups/.log", "w");
	if (!log)
	{
		printf("ERROR: Failed to open %s\n", "./backups/.log");
		exit(1);
	}

	log_wrapper(log, "Starting back up: ", argv[1]);

	file = fopen(argv[1], "r");
	if (!file)
	{
		printf("ERROR: Failed to open %s\n", argv[1]);
		exit(1);
	}

	strcpy(dest, "./backups/");
	strncat(dest, argv[1], 99 - strlen(dest));
	fd = open(dest, 193, 432LL);
	if (fd < 0)
	{
		printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
		exit(1);
	}
	
	while ((c = fgetc(file)) != -1)
		write(fd, &c, 1uLL);
	
	log_wrapper(log, "Finished back up ", argv[1]);
	fclose(file);
	close(fd);
	return 0;
}
