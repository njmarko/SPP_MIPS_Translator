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

.text
main:
	la	$t0, m1
	la	$t1, m10
	sw	$t0, 0($t1)
	li	$t1 ,6
	la	$t0, m4
	sw	$t1, 0($t0)
	la	$t0, m2
	li	$t0 ,5
	la	$t1, m8
	sw	$t0, 0($t1)
	li	$t0 ,1
	la	$t1, m6
	sw	$t0, 0($t1)
	li	$t0 ,0
	la	$t1, m5
	sw	$t0, 0($t1)
lab:
	la	$t0, m5
	lw	$t0, 0($t0)
	addi	$t0, $t0, 6
	la	$t1, m5
	sw	$t0, 0($t1)
	la	$t0, m7
	lw	$t1, 0($t0)
	xor	$t1, $t1, $t1
	la	$t0, m7
	sw	$t1, 0($t0)
	la	$t0, m6
	lw	$t0, 0($t0)
	la	$t1, m7
	lw	$t1, 0($t1)
	add	$t1, $t1, $t0
	la	$t0, m7
	sw	$t1, 0($t0)
	la	$t0, m7
	lw	$t1, 0($t0)
	la	$t0, m8
	lw	$t0, 0($t0)
	sub	$t1, $t1, $t0
	la	$t0, m7
	sw	$t1, 0($t0)
	la	$t0, m6
	lw	$t0, 0($t0)
	addi	$t0, $t0, 1
	la	$t1, m6
	sw	$t0, 0($t1)
	la	$t0, m7
	lw	$t1, 0($t0)
	bltz	$t1, lab
	la	$t1, m3
	la	$t0, m9
	sw	$t1, 0($t0)
	la	$t0, m5
	lw	$t0, 0($t0)
	la	$t1, m9
	lw	$t1, 0($t1)
	sw	$t0, 0($t1)
	nop
	li  $v0, 10
	syscall