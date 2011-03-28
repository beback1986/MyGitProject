	.file	"test.c"
	.text
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
	movl	$0, -4(%ebp)
	addl	$1, -4(%ebp)
	leave
	ret
	.size	func2, .-func2
.globl func3
	.type	func3, @function
func3:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	addl	$1, 8(%ebp)
	addl	$1, 12(%ebp)
	addl	$1, -4(%ebp)
	leave
	ret
	.size	func3, .-func3
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$24, %esp
	movl	$0, -16(%ebp)
	movl	$0, -12(%ebp)
	movl	$0, -8(%ebp)
	movl	$0, %ebx
	call	func1
	call	func2
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%ebx, (%esp)
	call	func3
	addl	$24, %esp
	popl	%ebx
	popl	%ebp
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.0 20090506 (Red Hat 4.4.0-4)"
	.section	.note.GNU-stack,"",@progbits
