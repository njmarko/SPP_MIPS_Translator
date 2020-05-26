.globl	main

.data
m1:		.word 6
m2:		.word 5
m3:		.word 0

.text
main:
	la	$t0, m1
	lw	$t3, 0($t0)
	la	$t0, m2
	lw	$t1, 0($t0)
	li	$t0 ,1
	li	$t4 ,0
lab:
	add	$t4, $t4, $t3
	sub	$t2, $t0, $t1
	addi	$t0, $t0, 1
	bltz	$t2, lab
	la	$t0, m3
	sw	$t4, 0($t0)
	nop
	li  $v0, 10
	syscall