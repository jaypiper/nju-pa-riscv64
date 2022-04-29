.section .text
.globl main

fact:
	addi sp, sp, -32
	sd ra, 0(sp)
	sd s0, 8(sp)
	sd s1, 16(sp)
	sd s2, 24(sp)
	li t1, 1
	beq a0, t1, label0
	j label1

label0:
	mv a0, a0
	ld ra, 0(sp)
	addi sp, sp, 32
	ret

label1:
	addi s0, a0, -1
	mv s1, a0
	mv a0, s0
	call fact
	mul s2, s1, a0
	mv a0, s2
	ld ra, 0(sp)
	ld s0, 8(sp)
	ld s1, 16(sp)
	ld s2, 24(sp)
	addi sp, sp, 32
	ret

main:
	addi sp, sp, -24
	sd ra, 0(sp)
	sd s0, 8(sp)
	sd s1, 16(sp)
	call my_read
	mv s0, a0
	li t1, 1
	bgt s0, t1, label2
	j label3

label2:
	mv a0, s0
	call fact
	mv s1, a0
	j label4

label3:
	li s1, 1

label4:
	mv a0, s1
	call my_write
	li a0, 0
	ld ra, 0(sp)
	ld s0, 8(sp)
	ld s1, 16(sp)
	addi sp, sp, 24
	ret
