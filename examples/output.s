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
	lw	$t0, 0($t0)
	la	$t1, m5
	sw	$t0, 0($t1)
	la	$t0, m2
	lw	$t1, 0($t0)
	la	$t0, m6
	sw	$t1, 0($t0)
	li	$t2 ,1
	li	$t1 ,0
	la	$t0, m4
	sw	$t1, 0($t0)
lab:
	la	$t0, m4
	lw	$t1, 0($t0)
	la	$t0, m5
	lw	$t0, 0($t0)
	add	$t1, $t1, $t0
	la	$t0, m4
	sw	$t1, 0($t0)
	la	$t0, m6
	lw	$t1, 0($t0)
	sub	$t0, $t2, $t1
	addi	$t2, $t2, 1
	bltz	$t0, lab
	la	$t0, m3
	la	$t1, m4
	lw	$t1, 0($t1)
	sw	$t1, 0($t0)
	nop
	li  $v0, 10
	syscall