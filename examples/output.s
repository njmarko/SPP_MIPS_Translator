.globl	main

.data
m1:		.word 6
m2:		.word 5
m3:		.word 0
m4:		.word 0
m5:		.word 0

.text
main:
	la	$t0, m1
	lw	$t0, 0($t0)
	la	$t1, m5
	sw	$t0, 0($t1)
	la	$t0, m2
	lw	$t1, 0($t0)
	li	$t3 ,1
	li	$t2 ,0
	la	$t0, m4
	sw	$t2, 0($t0)
lab:
	la	$t0, m4
	lw	$t2, 0($t0)
	la	$t0, m5
	lw	$t0, 0($t0)
	add	$t2, $t2, $t0
	la	$t0, m4
	sw	$t2, 0($t0)
	sub	$t0, $t3, $t1
	addi	$t3, $t3, 1
	bltz	$t0, lab
	la	$t1, m3
	la	$t0, m4
	lw	$t2, 0($t0)
	sw	$t2, 0($t1)
	nop
	li  $v0, 10
	syscall