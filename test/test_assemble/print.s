	.file	"test.c"
	.text

.globl print_integer
	.type	print_integer, @function
print_integer:
	pushl	%ebp
	movl	%esp, %ebp
#;	subl	$16, %esp
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	pushl	%eax

	movl	8(%ebp),  %edx # 参数三：字符串长度
	movl	12(%ebp), %ecx # 参数二：要显示的字符串

	movl	$1,       %ebx  # 参数一：文件描述符(stdout) 
	movl	$4,       %eax  # 系统调用号(sys_write) 
	int	$0x80           # 调用内核功能

	popl	%eax
	popl	%ebx
	popl	%ecx
	popl	%edx
	leave
	ret
	.size	print_integer, .-print_integer


.globl func1
	.type	func1, @function
func1:
	pushl	%ebp
	movl	%esp, %ebp
	popl	%ebp
	ret
	.size	func1, .-func1


.globl func2
	.type	func2, @function
func2:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$0, -16(%ebp)
	movl	$0, -12(%ebp)
	movl	$0, -8(%ebp)
	movl	$0, -4(%ebp)
	addl	$1, -16(%ebp)
	subl	$1, -12(%ebp)

	subl	$8, %esp
	movl	%ebp, -20(%ebp)	#string to display
	movl	$4, -24(%ebp)	#string length
	call	print_integer

	leave
	ret
	.size	func2, .-func2


.globl _start
#;	.type	_start, @function
_start:
#;	pushl	%ebp
#;	movl	%esp, %ebp
	call	func1
	call	func2

	movl $0,%ebx     # 参数一：退出代码
	movl $1,%eax     # 系统调用号(sys_exit) 
	int  $0x80       # 调用内核功能

#;	popl	%ebp
#;	ret
#;	.size	_start, .-_start
#;	.ident	"GCC: (GNU) 4.4.0 20090506 (Red Hat 4.4.0-4)"
#;	.section	.note.GNU-stack,"",@progbits
