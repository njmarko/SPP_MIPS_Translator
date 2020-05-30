.globl	main

.data
m1:		.word 6
m2:		.word 5
m3:		.word 0
m4:		.word 0
m5:		.word 0
m6:		.word 0
m7:		.word 0
m8:		.word 0
m9:		.word 0
m10:		.word 0
m11:		.word 0

.text
main:
	la	$t-1, m1
	la	$t-1, m11
	sw	$t-1, 0($t-1)
	la	$t0, m11
	lw	$t-1, 0($t0)
	lw	$t-1, 0($t-1)
	la	$t-1, m4
	sw	$t-1, 0($t-1)
	la	$t-1, m2
	la	$t-1, m10
	sw	$t-1, 0($t-1)
	la	$t0, m10
	lw	$t-1, 0($t0)
	lw	$t-1, 0($t-1)
	la	$t-1, m7
	sw	$t-1, 0($t-1)
	li	$t-1 ,1
	la	$t-1, m6
	sw	$t-1, 0($t-1)
	li	$t-1 ,0
	la	$t-1, m5
	sw	$t-1, 0($t-1)
lab:
	la	$t0, m4
	lw	$t-1, 0($t0)
	la	$t-1, m5
	lw	$t-1, 0($t-1)
	add	$t-1, $t-1, $t-1
	la	$t-1, m5
	sw	$t-1, 0($t-1)
	la	$t-1, m6
	lw	$t-1, 0($t-1)
	la	$t-1, m7
	lw	$t-1, 0($t-1)
	sub	$t-1, $t-1, $t-1
	la	$t-1, m8
	sw	$t-1, 0($t-1)
	la	$t-1, m6
	lw	$t-1, 0($t-1)
	addi	$t-1, $t-1, 1
	la	$t-1, m6
	sw	$t-1, 0($t-1)
	la	$t-1, m8
	lw	$t-1, 0($t-1)
	bltz	$t-1, lab
	la	$t-1, m3
	la	$t-1, m9
	sw	$t-1, 0($t-1)
	la	$t0, m5
	lw	$t-1, 0($t0)
	la	$t-1, m9
	lw	$t-1, 0($t-1)
	sw	$t-1, 0($t-1)
	nop
	li  $v0, 10
	syscall