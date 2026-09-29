# bomb

# GDB调试main

# 1

调试函数phase_1

```nasm
Dump of assembler code for function phase_1:
   0x0000000000400ee0 <+0>:	sub    $0x8,%rsp
   0x0000000000400ee4 <+4>:	mov    $0x402400,%esi
   0x0000000000400ee9 <+9>:	call   0x401338 <strings_not_equal>
```

这里将位置0x402400地址的数据压入到用于第二个参数的寄存器,然后调用string_not_equal函数

用x/s指令，以字符串方式解析0x402400这个地址

```nasm
(gdb) x/s 0x402400
0x402400:	"Border relations with Canada have never been better."
```

所以第一个字符串为

```nasm
Border relations with Canada have never been better.
```

# 2

调试函数phase_2

完整asm如下，一段一段看

```nasm
Dump of assembler code for function phase_2:
   0x0000000000400efc <+0>:	push   %rbp
   0x0000000000400efd <+1>:	push   %rbx
   0x0000000000400efe <+2>:	sub    $0x28,%rsp
   0x0000000000400f02 <+6>:	mov    %rsp,%rsi
   0x0000000000400f05 <+9>:	call   0x40145c <read_six_numbers>
   0x0000000000400f0a <+14>:	cmpl   $0x1,(%rsp)
   0x0000000000400f0e <+18>:	je     0x400f30 <phase_2+52>
   0x0000000000400f10 <+20>:	call   0x40143a <explode_bomb>
   0x0000000000400f15 <+25>:	jmp    0x400f30 <phase_2+52>
   0x0000000000400f17 <+27>:	mov    -0x4(%rbx),%eax
   0x0000000000400f1a <+30>:	add    %eax,%eax
   0x0000000000400f1c <+32>:	cmp    %eax,(%rbx)
   0x0000000000400f1e <+34>:	je     0x400f25 <phase_2+41>
   0x0000000000400f20 <+36>:	call   0x40143a <explode_bomb>
   0x0000000000400f25 <+41>:	add    $0x4,%rbx
   0x0000000000400f29 <+45>:	cmp    %rbp,%rbx
   0x0000000000400f2c <+48>:	jne    0x400f17 <phase_2+27>
   0x0000000000400f2e <+50>:	jmp    0x400f3c <phase_2+64>
   0x0000000000400f30 <+52>:	lea    0x4(%rsp),%rbx
   0x0000000000400f35 <+57>:	lea    0x18(%rsp),%rbp
   0x0000000000400f3a <+62>:	jmp    0x400f17 <phase_2+27>
   0x0000000000400f3c <+64>:	add    $0x28,%rsp
   0x0000000000400f40 <+68>:	pop    %rbx
   0x0000000000400f41 <+69>:	pop    %rbp
   0x0000000000400f42 <+70>:	ret
End of assembler dump.
```

### 2-Part1

```nasm
Dump of assembler code for function phase_2:
   0x0000000000400efc <+0>:	push   %rbp
   0x0000000000400efd <+1>:	push   %rbx
   0x0000000000400efe <+2>:	sub    $0x28,%rsp
   0x0000000000400f02 <+6>:	mov    %rsp,%rsi
   0x0000000000400f05 <+9>:	call   0x40145c <read_six_numbers>
   0x0000000000400f0a <+14>:	cmpl   $0x1,(%rsp)
   0x0000000000400f0e <+18>:	je     0x400f30 <phase_2+52>
   0x0000000000400f10 <+20>:	call   0x40143a <explode_bomb>
```

第一个参数还是传入的字符串，但是第二个参数就把栈指针传进去了，然后调用了`read_six_numbers`，内部具体是什么样的。

```nasm
(gdb) disassemble read_six_numbers
Dump of assembler code for function read_six_numbers:
   0x000000000040145c <+0>:	sub    $0x18,%rsp
   0x0000000000401460 <+4>:	mov    %rsi,%rdx
   0x0000000000401463 <+7>:	lea    0x4(%rsi),%rcx
   0x0000000000401467 <+11>:	lea    0x14(%rsi),%rax
   0x000000000040146b <+15>:	mov    %rax,0x8(%rsp)
   0x0000000000401470 <+20>:	lea    0x10(%rsi),%rax
   0x0000000000401474 <+24>:	mov    %rax,(%rsp)
   0x0000000000401478 <+28>:	lea    0xc(%rsi),%r9
   0x000000000040147c <+32>:	lea    0x8(%rsi),%r8
   0x0000000000401480 <+36>:	mov    $0x4025c3,%esi
   0x0000000000401485 <+41>:	mov    $0x0,%eax
   0x000000000040148a <+46>:	call   0x400bf0 <__isoc99_sscanf@plt>
   0x000000000040148f <+51>:	cmp    $0x5,%eax
   0x0000000000401492 <+54>:	jg     0x401499 <read_six_numbers+61>
   0x0000000000401494 <+56>:	call   0x40143a <explode_bomb>
   0x0000000000401499 <+61>:	add    $0x18,%rsp
   0x000000000040149d <+65>:	ret
End of assembler dump.
```

```nasm
(gdb) x/s 0x4025c3
0x4025c3:	"%d %d %d %d %d %d"
```

分配了`0x18=16+8=24`个字节，然后每4个字节作为一个`int`一共读取`6int`，这里读取使用`sscanf`来完成的

```c
sscanf(input, "%d %d %d %d %d %d", a[0], ... a[5]);
```

自`+51`开始，都是对`sscanf`的尾处理，`sscanf`返回真正读写到的变量个数，如果这个数字<=5，就直接`call bomb`

### 2-Part2

```asm
   0x0000000000400f0a <+14>:	cmpl   $0x1,(%rsp)
   0x0000000000400f0e <+18>:	je     0x400f30 <phase_2+52>
   0x0000000000400f10 <+20>:	call   0x40143a <explode_bomb>
   0x0000000000400f15 <+25>:	jmp    0x400f30 <phase_2+52>
   ----------------------------------------------------------
   0x0000000000400f17 <+27>:	mov    -0x4(%rbx),%eax
   0x0000000000400f1a <+30>:	add    %eax,%eax
   0x0000000000400f1c <+32>:	cmp    %eax,(%rbx)
   0x0000000000400f1e <+34>:	je     0x400f25 <phase_2+41>
   0x0000000000400f20 <+36>:	call   0x40143a <explode_bomb>
   0x0000000000400f25 <+41>:	add    $0x4,%rbx
   0x0000000000400f29 <+45>:	cmp    %rbp,%rbx
   0x0000000000400f2c <+48>:	jne    0x400f17 <phase_2+27>
   0x0000000000400f2e <+50>:	jmp    0x400f3c <phase_2+64>
   -----------------------------------------------------------
   0x0000000000400f30 <+52>:	lea    0x4(%rsp),%rbx
   0x0000000000400f35 <+57>:	lea    0x18(%rsp),%rbp
   0x0000000000400f3a <+62>:	jmp    0x400f17 <phase_2+27>
```

这里才是判断的核心部分

看到`+14`，判断栈顶是否与1相等，刚才读入了6个数字，这里栈顶指向了其首元素，就是`a[0]`，所以第一个数一定是`1`，否则引爆炸弹。

判断成功了就去了`+52`，这里把`rsp+4`给了`rbx`，就是取出`a[1]`，`+57`将基准指针设置到数组后面。

还是先看中间部分吧。

```nasm
   0x0000000000400f17 <+27>:	mov    -0x4(%rbx),%eax
   0x0000000000400f1a <+30>:	add    %eax,%eax
```

`+27`负责将上一个元素搬给`eax`，一开始是1，随后`eax+=eax`，变成2，然后在判等，比的是自增后的`eax`和`rbx`，也就是`a[i]`与`a[i-1]`，这里可以看出，必须满足`a[i]=2*a[i-1]`才不会爆炸。

```nasm
   0x0000000000400f29 <+45>:	cmp    %rbp,%rbx
   0x0000000000400f2c <+48>:	jne    0x400f17 <phase_2+27>
   0x0000000000400f2e <+50>:	jmp    0x400f3c <phase_2+64>
```

判断数组有没有遍历完成，遍历完成就结束，没有就继续执行`+52`

不难得到，

第一个答案是`Border relations with Canada have never been better.`

第二个答案是`1 2 4 8 16 32`

# 3

调试函数phase_3，asm码如下

```nasm
(gdb) disassemble phase_3
Dump of assembler code for function phase_3:
   0x00400f43 <+0>:     sub    $0x18,%rsp
   0x00400f47 <+4>:     lea    0xc(%rsp),%rcx
rsp+c(12)分给第4个参数,rsp+8分给第3个参数
   0x00400f4c <+9>:     lea    0x8(%rsp),%rdx
   0x00400f51 <+14>:    mov    $0x4025cf,%esi
--------------------------------------------------
(gdb) x/s 0x4025cf
0x4025cf:       "%d %d"
--------------------------------------------------
   0x00400f56 <+19>:    mov    $0x0,%eax
   0x00400f5b <+24>:    call   0x400bf0 <__isoc99_sscanf@plt>
--------------------------------------------------
sscanf(input, "%d %d", &x1, &x2);
下面记第一个输入的数字是x1，第二个是x2
--------------------------------------------------
   ----------------------------------------------------------
   0x00400f60 <+29>:    cmp    $0x1,%eax
   判断sscanf返回值是否符合预期
   0x00400f63 <+32>:    jg     0x400f6a <phase_3+39>
   如果没有输入两个参数，引爆炸弹
   0x00400f65 <+34>:    call   0x40143a <explode_bomb>
   ----------------------------------------------------------
   0x00400f6a <+39>:    cmpl   $0x7,0x8(%rsp)
   ja:无符号大于,如果x1>7，跳转到106引爆
   0x00400f6f <+44>:    ja     0x400fad <phase_3+106>
   所以x1<=7
   0x00400f71 <+46>:    mov    0x8(%rsp),%eax
   0x00400f75 <+50>:    jmp    *0x402470(,%rax,8)
   我们对x1已经进行不等式的约束，那么，根据x1跳转地址，就是
   target=0x402470+8*x1
   (gdb) x/8gx 0x402470
0x402470:       0x0000000000400f7c      0x0000000000400fb9
0x402480:       0x0000000000400f83      0x0000000000400f8a
0x402490:       0x0000000000400f91      0x0000000000400f98
0x4024a0:       0x0000000000400f9f      0x0000000000400fa6
 0      0x402470       0x400f7c 207
 1      0x402478       0x400fb9 311
 2      0x402480       0x400f83 707
 3      0x402488       0x400f8a 256
 4      0x402490       0x400f91 389
 5      0x402498       0x400f98 206
 6      0x4024a0       0x400f9f 682
 7      0x4024a8       0x400fa6 327
   --------------------------------------------------
   0x00400f7c <+57>:    mov    $0xcf,%eax
   0x00400f81 <+62>:    jmp    0x400fbe <phase_3+123>
   0x00400f83 <+64>:    mov    $0x2c3,%eax
   0x00400f88 <+69>:    jmp    0x400fbe <phase_3+123>
   0x00400f8a <+71>:    mov    $0x100,%eax
   0x00400f8f <+76>:    jmp    0x400fbe <phase_3+123>
   0x00400f91 <+78>:    mov    $0x185,%eax
   0x00400f96 <+83>:    jmp    0x400fbe <phase_3+123>
   0x00400f98 <+85>:    mov    $0xce,%eax
   0x00400f9d <+90>:    jmp    0x400fbe <phase_3+123>
   0x00400f9f <+92>:    mov    $0x2aa,%eax
   0x00400fa4 <+97>:    jmp    0x400fbe <phase_3+123>
   0x00400fa6 <+99>:    mov    $0x147,%eax
   0x00400fab <+104>:   jmp    0x400fbe <phase_3+123>
   ------------------------------------------------------
   0x00400fad <+106>:   call   0x40143a <explode_bomb>
   0x00400fb2 <+111>:   mov    $0x0,%eax
   0x00400fb7 <+116>:   jmp    0x400fbe <phase_3+123>
   0x00400fb9 <+118>:   mov    $0x137,%eax
   0x00400fbe <+123>:   cmp    0xc(%rsp),%eax
   0x00400fc2 <+127>:   je     0x400fc9 <phase_3+134>
   0x00400fc4 <+129>:   call   0x40143a <explode_bomb>
   0x00400fc9 <+134>:   add    $0x18,%rsp
   0x00400fcd <+138>:   ret
```

第一个答案是`Border relations with Canada have never been better.`

第二个答案是`1 2 4 8 16 32`

第三个答案是

`0 207、1 311、2 707、3 256、4 389、5 206、6 682、7 327`中任选一个