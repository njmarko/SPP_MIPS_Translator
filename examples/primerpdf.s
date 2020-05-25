.globl main
.data
m1: .word 6
m2: .word 5
.text
main:
la $t0, m1
lw $t1, 0($t0)
la $t0, m2
lw $t2, 0($t0)
add $t0, $t1, $t2