.globl	main

.data
m1:		.word 6
m2:		.word 5
m3:		.word 0
m4:		.word 0
m5:		.word 0
m6:		.word 0

.text
main:
	la	$t0, m1
	lw	$t2, 0($t0)
	la	$t0, m5
	sw	$t2, 0($t0)
	la	$t0, m2
	lw	$t2, 0($t0)
	la	$t0, m6
	sw	$t2, 0($t0)
	li	$t1 ,1
	li	$t0 ,0
	la	$t2, m4
	sw	$t0, 0($t2)
lab:
	la	$t0, m4
	lw	$t0, 0($t0)
	la	$t2, m5
	lw	$t2, 0($t2)
	add	$t0, $t0, $t2
	la	$t2, m4
	sw	$t0, 0($t2)
	la	$t0, m6
	lw	$t2, 0($t0)
	sub	$t0, $t1, $t2
	addi	$t1, $t1, 1
	bltz	$t0, lab
	la	$t1, m3
	la	$t0, m4
	lw	$t0, 0($t0)
	sw	$t0, 0($t1)
	nop
	li  $v0, 10
	syscall