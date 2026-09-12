# bomb

# GDB调试main

# 1

调试函数phase_1

```asm
Dump of assembler code for function phase_1:
   0x0000000000400ee0 <+0>:	sub    $0x8,%rsp
   0x0000000000400ee4 <+4>:	mov    $0x402400,%esi
   0x0000000000400ee9 <+9>:	call   0x401338 <strings_not_equal>
```

这里将位置0x402400地址的数据压入到用于第二个参数的寄存器,然后调用string_not_equal函数

用x/s指令，以字符串方式解析0x402400这个地址

```asm
(gdb) x/s 0x402400
0x402400:	"Border relations with Canada have never been better."
```

所以第一个字符串为

```asm
Border relations with Canada have never been better.
```

