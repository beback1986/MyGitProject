# args.s
.text
.globl _start
        
_start:
        popl	%ecx		# argc
vnext:
        popl	%ecx		# argv
        test 	%ecx, %ecx      # 空指针表明结束
        jz	exit
        movl	%ecx, %ebx
        xorl	%edx, %edx
strlen:
        movb	(%ebx), %al
        inc	%edx
        inc	%ebx
        test	%al, %al
        jnz	strlen
        movb	$10, -1(%ebx)
        movl	$4, %eax        # 系统调用号(sys_write) 
        movl	$1, %ebx        # 文件描述符(stdout) 
        int	$0x80
        jmp	vnext
exit:
        movl	$1,%eax         # 系统调用号(sys_exit) 
        xorl	%ebx, %ebx      # 退出代码
        int 	$0x80
		
        ret

