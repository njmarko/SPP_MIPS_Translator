.globl	main

.data
m1:		.word 6
m2:		.word 5
m3:		.word 0
m4:		.word 0

.text
main:
	la	$t0, m1
	lw	$t2, 0($t0)
	la	$t0, m4
	sw	$t2, 0($t0)
	la	$t0, m2
	lw	$t3, 0($t0)
	li	$t0 ,1
	li	$t1 ,0
lab:
	la	$t2, m4
	lw	$t2, 0($t2)
	add	$t1, $t1, $t2
	sub	$t2, $t0, $t3
	addi	$t0, $t0, 1
	bltz	$t2, lab
	la	$t0, m3
	sw	$t1, 0($t0)
	nop
	li  $v0, 10
	syscall