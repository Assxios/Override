#include <stdio.h>

void log_wrapper(FILE *file, const char *str, const char *name)
{
	char buffer[264];
	strcpy(buffer, str);
	snprintf(&buffer[strlen(buffer)], 254 - strlen(buffer), name);

	buffer[strcspn(buffer, "\n")] = 0;
	fprintf(file, "LOG: %s\n", buffer);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		printf("Usage: %s filename\n", *argv);

	FILE *log = fopen("./backups/.log", "w");
	if (!log)
	{
		printf("ERROR: Failed to open %s\n", "./backups/.log");
		exit(1);
	}

	log_wrapper(log, "Starting back up: ", argv[1]);

	FILE *file = fopen(argv[1], "r");
	if (!file)
	{
		printf("ERROR: Failed to open %s\n", argv[1]);
		exit(1);
	}
	
	char dest[104];
	strcpy(dest, "./backups/");
	strncat(dest, argv[1], 99 - strlen(dest));

	int fd = open(dest, 193, 432);
	if (fd < 0)
	{
		printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
		exit(1);
	}

	while (1)
	{
		char buf = fgetc(file);
		if (buf == -1)
			break;
		write(fd, &buf, 1);
	}

	log_wrapper(log, "Finished back up ", argv[1]);
	fclose(file);
	close(fd);
	return 0;
}
