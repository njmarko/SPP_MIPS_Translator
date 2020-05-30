.globl	main

.data
m1:		.word 6
m2:		.word 5
m3:		.word 0
m4:		.word 0
m5:		.word 0
m6:		.word 0
m7:		.word 0

.text
main:
	la	$t0, m1
	lw	$t2, 0($t0)
	la	$t0, m4
	sw	$t2, 0($t0)
	la	$t0, m2
	lw	$t1, 0($t0)
	li	$t0 ,1
	la	$t2, m6
	sw	$t0, 0($t2)
	li	$t0 ,0
	la	$t2, m5
	sw	$t0, 0($t2)
lab:
	la	$t0, m4
	lw	$t2, 0($t0)
	la	$t0, m5
	lw	$t0, 0($t0)
	add	$t0, $t0, $t2
	la	$t2, m5
	sw	$t0, 0($t2)
	la	$t0, m7
	lw	$t2, 0($t0)
	xor	$t2, $t2, $t2
	la	$t0, m7
	sw	$t2, 0($t0)
	la	$t0, m6
	lw	$t0, 0($t0)
	la	$t2, m7
	lw	$t2, 0($t2)
	add	$t2, $t2, $t0
	la	$t0, m7
	sw	$t2, 0($t0)
	la	$t0, m7
	lw	$t2, 0($t0)
	sub	$t2, $t2, $t1
	la	$t0, m7
	sw	$t2, 0($t0)
	la	$t0, m6
	lw	$t0, 0($t0)
	addi	$t0, $t0, 1
	la	$t2, m6
	sw	$t0, 0($t2)
	la	$t0, m7
	lw	$t2, 0($t0)
	bltz	$t2, lab
	la	$t1, m3
	la	$t0, m5
	lw	$t0, 0($t0)
	sw	$t0, 0($t1)
	nop
	li  $v0, 10
	syscall