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
	li	$t0 ,6
	la	$t0, m2
	li	$t0 ,5
	la	$t1, m5
	sw	$t0, 0($t1)
	li	$t1 ,1
	la	$t0, m6
	sw	$t1, 0($t0)
	li	$t1 ,0
	la	$t0, m4
	sw	$t1, 0($t0)
lab:
	la	$t0, m4
	lw	$t1, 0($t0)
	addi	$t1, $t1, 6
	la	$t0, m4
	sw	$t1, 0($t0)
	la	$t0, m5
	lw	$t0, 0($t0)
	neg	$t0, $t0
	la	$t1, m7
	sw	$t0, 0($t1)
	la	$t0, m6
	lw	$t1, 0($t0)
	la	$t0, m7
	lw	$t0, 0($t0)
	add	$t0, $t0, $t1
	la	$t1, m7
	sw	$t0, 0($t1)
	la	$t0, m6
	lw	$t1, 0($t0)
	addi	$t1, $t1, 1
	la	$t0, m6
	sw	$t1, 0($t0)
	la	$t0, m7
	lw	$t0, 0($t0)
	bltz	$t0, lab
	la	$t0, m3
	la	$t1, m4
	lw	$t1, 0($t1)
	sw	$t1, 0($t0)
	nop	
	li  $v0, 10
	syscall