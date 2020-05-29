.globl	main

.data
m1:		.word 6
m2:		.word 5
m3:		.word 0

.text
	la	$t1, m1
	lw	$t3, 0($t1)
	la	$t0, m2
	lw	$t2, 0($t0)
main:
	la	$t1, m1
	lw	$t3, 0($t1)
	la	$t0, m2
	lw	$t2, 0($t0)
	add	$t0, $t3, $t2
	la	$t1, m3
	sw	$t0, 0($t1)
	mul	$t0, $t3, $t2
	la	$t1, m3
	sw	$t0, 0($t1)
	xor	$t0, $t0, $t0
	li  $v0, 10
	syscall