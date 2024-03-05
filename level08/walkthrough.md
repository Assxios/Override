check the c code to have more explaination about what it does

the code take user input to a file and read it, it then save the following information to ./backups/argv[1]

to match this we can:
- make a backups folder and all the argv[1] folder in it
- use the .pass file

in the tmp we can create the folders and run the elf:

    $ cd /tmp
    $ mkdir -p backups/home/users/level09
    $ ~/level08 ~level09/.pass

then just cat the save

    $ cat /tmp/backups/home/users/level09/.pass
    fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
