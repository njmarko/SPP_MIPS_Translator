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
	lw	$t2, 0($t0)
	li	$t1 ,1
	li	$t4 ,0
lab:
	add	$t4, $t4, $t3
	sub	$t0, $t1, $t2
	addi	$t1, $t1, 1
	bltz	$t0, lab
	la	$t0, m3
	sw	$t4, 0($t0)
	nop
	li  $v0, 10
	syscall