.globl	main

.data
m1:		.word 6
m2:		.word 5
m3:		.word 0
str:	.asciiz "\novo je rezultat operacije add: "
.text
main:
	la	$t1, m1
	lw	$t3, 0($t1)
	la	$t0, m2
	lw	$t2, 0($t0)
	add	$t0, $t3, $t2
	la	$t1, m3
	sw	$t0, 0($t1)
	mul	$t0, $t3, $t2
	li $v0, 4       # $system call code for print_str
    la $a0, str     # $address of string to print
    syscall         # print the string
	li $v0, 1       # $system call code for print_int
	addi $a0, $t0, 0
    syscall         # print it
	la	$t1, m3
	sw	$t0, 0($t1)

	li  $v0, 10
	syscall