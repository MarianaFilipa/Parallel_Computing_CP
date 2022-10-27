	.file	"k_means_Descartada.c"
	.text
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"x = %f / y = %f / nCluster = %d \n"
	.text
	.p2align 4
	.globl	printPoint
	.type	printPoint, @function
printPoint:
.LFB39:
	.cfi_startproc
	endbr64
	movl	8(%rdi), %edx
	pxor	%xmm0, %xmm0
	pxor	%xmm1, %xmm1
	movl	$2, %eax
	cvtss2sd	(%rdi), %xmm0
	cvtss2sd	4(%rdi), %xmm1
	leaq	.LC0(%rip), %rsi
	movl	$1, %edi
	jmp	__printf_chk@PLT
	.cfi_endproc
.LFE39:
	.size	printPoint, .-printPoint
	.p2align 4
	.globl	createArrayPoints
	.type	createArrayPoints, @function
createArrayPoints:
.LFB40:
	.cfi_startproc
	endbr64
	movl	$80000000, %edi
	jmp	malloc@PLT
	.cfi_endproc
.LFE40:
	.size	createArrayPoints, .-createArrayPoints
	.p2align 4
	.globl	createArrayCentroids
	.type	createArrayCentroids, @function
createArrayCentroids:
.LFB41:
	.cfi_startproc
	endbr64
	movl	$32, %edi
	jmp	malloc@PLT
	.cfi_endproc
.LFE41:
	.size	createArrayCentroids, .-createArrayCentroids
	.p2align 4
	.globl	init
	.type	init, @function
init:
.LFB42:
	.cfi_startproc
	endbr64
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	movq	%rsi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movq	%rdi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	leaq	80000000(%rdi), %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movq	%rdi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 48
	.p2align 4,,10
	.p2align 3
.L6:
	movl	$12, %edi
	addq	$8, %rbx
	call	malloc@PLT
	movq	%rax, -8(%rbx)
	call	rand@PLT
	pxor	%xmm0, %xmm0
	movq	-8(%rbx), %rdx
	cvtsi2ssl	%eax, %xmm0
	mulss	.LC1(%rip), %xmm0
	movss	%xmm0, (%rdx)
	call	rand@PLT
	pxor	%xmm0, %xmm0
	movq	-8(%rbx), %rdx
	cvtsi2ssl	%eax, %xmm0
	mulss	.LC1(%rip), %xmm0
	movl	$-1, 8(%rdx)
	movss	%xmm0, 4(%rdx)
	cmpq	%rbx, %rbp
	jne	.L6
	xorl	%ebx, %ebx
.L7:
	movl	$8, %edi
	call	malloc@PLT
	movq	(%r12,%rbx), %rdx
	movq	%rax, 0(%r13,%rbx)
	addq	$8, %rbx
	movss	(%rdx), %xmm0
	movss	%xmm0, (%rax)
	movss	4(%rdx), %xmm0
	movss	%xmm0, 4(%rax)
	cmpq	$32, %rbx
	jne	.L7
	addq	$8, %rsp
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE42:
	.size	init, .-init
	.p2align 4
	.globl	determineDistance
	.type	determineDistance, @function
determineDistance:
.LFB43:
	.cfi_startproc
	endbr64
	movss	(%rdi), %xmm0
	movss	4(%rdi), %xmm1
	subss	(%rsi), %xmm0
	subss	4(%rsi), %xmm1
	mulss	%xmm0, %xmm0
	mulss	%xmm1, %xmm1
	addss	%xmm1, %xmm0
	ret
	.cfi_endproc
.LFE43:
	.size	determineDistance, .-determineDistance
	.p2align 4
	.globl	update_cluster_points
	.type	update_cluster_points, @function
update_cluster_points:
.LFB44:
	.cfi_startproc
	endbr64
	movss	.LC2(%rip), %xmm5
	movq	%rsi, %r8
	leaq	80000000(%rdi), %r10
	xorl	%r9d, %r9d
	.p2align 4,,10
	.p2align 3
.L13:
	movq	(%rdi), %rsi
	xorl	%eax, %eax
	movaps	%xmm5, %xmm2
	movss	(%rsi), %xmm4
	movss	4(%rsi), %xmm3
.L16:
	movq	(%r8,%rax,8), %rdx
	movaps	%xmm4, %xmm0
	movaps	%xmm3, %xmm1
	subss	(%rdx), %xmm0
	subss	4(%rdx), %xmm1
	mulss	%xmm0, %xmm0
	mulss	%xmm1, %xmm1
	addss	%xmm1, %xmm0
	comiss	%xmm0, %xmm2
	minss	%xmm2, %xmm0
	cmova	%eax, %ecx
	addq	$1, %rax
	movaps	%xmm0, %xmm2
	cmpq	$4, %rax
	jne	.L16
	cmpl	%ecx, 8(%rsi)
	je	.L17
	movl	%ecx, 8(%rsi)
	addl	$1, %r9d
.L17:
	addq	$8, %rdi
	cmpq	%rdi, %r10
	jne	.L13
	movl	%r9d, %eax
	ret
	.cfi_endproc
.LFE44:
	.size	update_cluster_points, .-update_cluster_points
	.p2align 4
	.globl	determine_new_centroid
	.type	determine_new_centroid, @function
determine_new_centroid:
.LFB45:
	.cfi_startproc
	endbr64
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	xorl	%r14d, %r14d
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movq	%rdi, %r12
	movl	$32, %edi
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdx, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rsi, %rbx
	call	malloc@PLT
	movq	%rax, %r13
.L22:
	movl	$8, %edi
	call	malloc@PLT
	movl	$0, (%r12,%r14,4)
	movq	%rax, 0(%r13,%r14,8)
	addq	$1, %r14
	movq	$0, (%rax)
	cmpq	$4, %r14
	jne	.L22
	movq	%rbx, %rsi
	leaq	80000000(%rbx), %rdi
	.p2align 4,,10
	.p2align 3
.L23:
	movq	(%rsi), %rcx
	addq	$8, %rsi
	movslq	8(%rcx), %rdx
	movq	0(%r13,%rdx,8), %rax
	movss	(%rax), %xmm0
	addss	(%rcx), %xmm0
	movss	%xmm0, (%rax)
	movss	4(%rax), %xmm0
	addss	4(%rcx), %xmm0
	addl	$1, (%r12,%rdx,4)
	movss	%xmm0, 4(%rax)
	cmpq	%rsi, %rdi
	jne	.L23
	xorl	%eax, %eax
.L24:
	pxor	%xmm1, %xmm1
	movq	0(%r13,%rax,8), %rcx
	movq	0(%rbp,%rax,8), %rdx
	cvtsi2ssl	(%r12,%rax,4), %xmm1
	addq	$1, %rax
	movss	(%rcx), %xmm0
	divss	%xmm1, %xmm0
	movss	%xmm0, (%rdx)
	movss	4(%rcx), %xmm0
	divss	%xmm1, %xmm0
	movss	%xmm0, 4(%rdx)
	cmpq	$4, %rax
	jne	.L24
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE45:
	.size	determine_new_centroid, .-determine_new_centroid
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC3:
	.string	"%d\n"
	.section	.rodata.str1.8
	.align 8
.LC4:
	.string	"Center: (%.3f,%.3f) : Size %d \n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB46:
	.cfi_startproc
	endbr64
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	movl	$80000000, %edi
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	xorl	%r12d, %r12d
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$40, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	movq	%rsp, %r13
	call	malloc@PLT
	movl	$32, %edi
	movq	%rax, %rbp
	call	malloc@PLT
	movq	%rbp, %rdi
	movq	%rax, %rbx
	movq	%rax, %rsi
	call	init
	movq	%rbx, %rsi
	movq	%rbp, %rdi
	call	update_cluster_points
	.p2align 4,,10
	.p2align 3

	//ciclo while?
.L30:
	movq	%rbx, %rdx
	movq	%rbp, %rsi
	movq	%r13, %rdi
	addl	$1, %r12d
	call	determine_new_centroid
	movq	%rbx, %rsi
	movq	%rbp, %rdi
	call	update_cluster_points
	testl	%eax, %eax
	jne	.L30
	movl	%r12d, %edx
	leaq	.LC3(%rip), %rsi
	movl	$1, %edi
	xorl	%ebp, %ebp
	call	__printf_chk@PLT
	leaq	.LC4(%rip), %r12
.L31:
	movq	(%rbx,%rbp,8), %rax //Carregar os valores de 
	movl	0(%r13,%rbp,4), %edx
	pxor	%xmm0, %xmm0
	movq	%r12, %rsi
	pxor	%xmm1, %xmm1
	movl	$1, %edi
	addq	$1, %rbp
	cvtss2sd	(%rax), %xmm0
	cvtss2sd	4(%rax), %xmm1
	movl	$2, %eax
	call	__printf_chk@PLT
	cmpq	$4, %rbp
	jne	.L31
	movq	24(%rsp), %rax
	xorq	%fs:40, %rax
	jne	.L36
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
.L36:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE46:
	.size	main, .-main
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC1:
	.long	805306368
	.align 4
.LC2:
	.long	1120403456
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
