createArrayPoints:
	movl	$80000000, %edi 	| o array de pontos está aqui
createArrayCentroids:
	movl	$32, %edi 			| o array de centroids está aqui

----------------------------------------------------
void init(Point *array_points, Point *array_centroids)

	movq	%rsi, %r13

	movq	%rdi, %r12

	leaq	80000000(%rdi), %rbp 	| carrega N para %rbp

	movq	%rdi, %rbx 			
	subq	$8, %rsp


--------------------------------------------------
    for (i = 0; i < N; i++)
    {
        array_points[i] = (Point)malloc(sizeof(struct point));
        array_points[i]->x = (float)rand() / RAND_MAX;
        array_points[i]->y = (float)rand() / RAND_MAX;
        array_points[i]->nCluster = -1;
    }

.L6:								| %rdx é o array_points[i] 
	movl	$12, %edi				
	addq	$8, %rb					| %rb é o apontador do array
	call	malloc@PLT				| faz malloc no %rb
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
	movl	$-1, 8(%rdx)			|array_points[i]->nCluster = -1;
	movss	%xmm0, 4(%rdx)
	cmpq	%rbx, %rbp				| i está em %rbx
	jne	.L6							
	xorl	%ebx, %ebx

--------------------------------------------------
    for (j = 0; j < K; j++)
    {
        array_centroids[j] = (Point)malloc(sizeof(struct point));
        array_centroids[j]->x = array_points[j]->x;
        array_centroids[j]->y = array_points[j]->y;
        array_centroids[j]->nCluster = -1;
    }

.L7:
	movl	$12, %edi				
	call	malloc@PLT			
	movq	%rax, 0(%r13,%rbx)	| 
	movq	(%r12,%rbx), %rdx	| carrega array_points para %rdx
	addq	$8, %rbx			| i que está em %rbx é atualizado
	movss	(%rdx), %xmm0		| %rdx => era array_points[j]->x
	movss	%xmm0, (%rax)		| array_centroids[j]->x está em (%rax)  array_points[j]->x;(%xmm0)
	movss	4(%rdx), %xmm0		| carrega para %xmm0 o valor de array_points[j]->y
	movl	$-1, 8(%rax)		| array_centroids[j]->nCluster está em 8(%rax)
	movss	%xmm0, 4(%rax)		| 4(%rax) é o array_centroids[j]->y
	cmpq	$32, %rbx			| %rbx é o i
	jne	.L7

----------------------------------------------------
update_cluster_points:
.LFB44:
	.cfi_startproc
	endbr64
	movss	.LC2(%rip), %xmm5
	movq	%rsi, %r8
	leaq	80000000(%rdi), %r10	| carrega o N para %r10 
	xorl	%r9d, %r9d
.L13:
	movq	(%rdi), %rsi
	xorl	%eax, %eax
	movaps	%xmm5, %xmm2
	movss	(%rsi), %xmm4
	movss	4(%rsi), %xmm3
.L16:
	movq	(%r8,%rax,8), %rdx

	Determine Distance
	----------
	movaps	%xmm4, %xmm0
	movaps	%xmm3, %xmm1
	subss	(%rdx), %xmm0
	subss	4(%rdx), %xmm1
	mulss	%xmm0, %xmm0
	mulss	%xmm1, %xmm1
	addss	%xmm1, %xmm0
	-----------
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
.LFE44:
	.size	update_cluster_points, .-update_cluster_points
	.p2align 4
	.globl	determine_new_centroid
	.type	determine_new_centroid, @function


-----------------------------------------------------------------------
determine_new_centroid:
.L22:
-------------------------------------------
for (i = 0; i < K; i++)
    {
        allcentroids[i]->x = 0;
        allcentroids[i]->y = 0;
        size[i] = 0;
    }

	movq	(%rdx,%rax,8), %rcx
	movq	$0, (%rcx)
	movl	$0, (%rdi,%rax,4)
	addq	$1, %rax
	cmpq	$4, %rax
	jne	.L22
	
-----------------------------------------------
for (i = 0; i < N; i++)
    {
        int nCluster = allpoints[i]->nCluster;
        allcentroids[nCluster]->x += allpoints[i]->x;
        allcentroids[nCluster]->y += allpoints[i]->y;
        size[nCluster]++;
    }

Antes:
	leaq	80000000(%rsi), %r9 		| Carrega N para %r9
										| Carrega I para %rax
										| Carrega Size array para (%rdi,%rax,4)
										| Carrega CENTROIDS ARRAY para (%rdx,%rax,8)


.L23:									| %r9 é N	| (%rdi,%rcx,4) é o size array
	movq	(%rsi), %r8 				| %r8 é allpoints[i]
	addq	$8, %rsi					| i em %rsi
	movslq	8(%r8), %rcx				| nCluster em %rcx; |nCluster = allpoints[i]->nCluster;
	movq	(%rdx,%rcx,8), %rax 		| carrega o allcentroids[i]
	movss	(%rax), %xmm0				| pega no allcentroids[i]->x
	addss	(%r8), %xmm0 				| allcentroids[nCluster]->x += allpoints[i]->x;
	movss	%xmm0, (%rax)				| atualiza allcentroids[i]->x
	movss	4(%rax), %xmm0				| carrega allcentroids[i]->y
	addss	4(%r8), %xmm0				| allcentroids[nCluster]->y += allpoints[i]->y;
	movss	%xmm0, 4(%rax)				| atualiza allcentroids[i]->y
	addl	$1, (%rdi,%rcx,4) 			| os valores de size são alterados diretamente na memória
	cmpq	%rsi, %r9 					| i é comparado com N
	jne	.L23
	xorl	%eax, %eax

--------------------------------------------------
for (i = 0; i < K; i++)
    {
        allcentroids[i]->x = allcentroids[i]->x / size[i];
        allcentroids[i]->y = allcentroids[i]->y / size[i];
    }

.L24:
	pxor	%xmm1, %xmm1					| i está em %rax
	movq	(%rdx,%rax,8), %rcx				| carrega allcentroids[i] para %rcx
	cvtsi2ssl	(%rdi,%rax,4), %xmm1		| carrega o size[i] em %xmm1
	addq	$1, %rax						| atualiza o valor de i
	movss	(%rcx), %xmm0					| carrega allcentroids[i]->x para %xmm0
	divss	%xmm1, %xmm0					| allcentroids[i]->x / size[i];
	movss	%xmm0, (%rcx)					| guarda em allcentroids[i]->x o res da div
	movss	4(%rcx), %xmm0					| carrega allcentroids[i]->y para %xmm0
	divss	%xmm1, %xmm0					| allcentroids[i]->y / size[i]
	movss	%xmm0, 4(%rcx)					| guarda em allcentroids[i]->y o res da div
	cmpq	$4, %rax						| i está em %rax
	jne	.L24
	ret
	.cfi_endproc


main:
.LFB46:
	.cfi_startproc
	endbr64
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	xorl	%r12d, %r12d
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$48, %rsp
	.cfi_def_cfa_offset 96
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %r13
	call	clock@PLT
	movl	$80000000, %edi
	movq	%rax, %r14
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
.L29:
	movq	%rbx, %rdx
	movq	%rbp, %rsi
	movq	%r13, %rdi
	addl	$1, %r12d
	call	determine_new_centroid
	movq	%rbx, %rsi
	movq	%rbp, %rdi
	call	update_cluster_points
	testl	%eax, %eax
	jne	.L29
	call	clock@PLT
	pxor	%xmm0, %xmm0
	movl	%r12d, %edx
	xorl	%ebp, %ebp
	subq	%r14, %rax
	leaq	.LC4(%rip), %rsi
	movl	$1, %edi
	cvtsi2sdq	%rax, %xmm0
	xorl	%eax, %eax
	divsd	.LC3(%rip), %xmm0
	leaq	.LC5(%rip), %r12
	movsd	%xmm0, 8(%rsp)
	call	__printf_chk@PLT
.L30:
	movq	(%rbx,%rbp,8), %rax
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
	jne	.L30
	movsd	8(%rsp), %xmm0
	movl	$1, %edi
	movl	$1, %eax
	leaq	.LC6(%rip), %rsi
	call	__printf_chk@PLT
	movq	40(%rsp), %rax
	xorq	%fs:40, %rax
	jne	.L35
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
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
.L35:
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
