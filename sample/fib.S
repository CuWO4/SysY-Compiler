	.data
	.global fib_arr
fib_arr:
	.zero 40
	.text
	.global main
fib:
	addi    sp, sp, -96
	sw      ra, 92(sp)
LLB_5:
	addi    t0, sp, 52
	sw      t0, 56(sp)
	lw      t0, 56(sp)
	mv      t1, a0
	sw      t1, 0(t0)
	lw      t0, 56(sp)
	lw      t1, 0(t0)
	sw      t1, 44(sp)
	lw      t0, 44(sp)
	li      t1, 1
	sgt     t2, t0, t1
	xori    t0, t2, 1
	sw      t0, 16(sp)
	lw      t0, 16(sp)
	bnez    t0, LLB_0
	j       LLB_1
LLB_0:
	lw      t0, 56(sp)
	lw      t1, 0(t0)
	sw      t1, 40(sp)
	lw      t0, 40(sp)
	mv      a0, t0
	lw      ra, 92(sp)
	addi    sp, sp, 96
	ret     
LLB_1:
	lw      t0, 56(sp)
	lw      t1, 0(t0)
	sw      t1, 32(sp)
	la      t0, fib_arr
	lw      t1, 32(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 28(sp)
	lw      t0, 28(sp)
	lw      t1, 0(t0)
	sw      t1, 36(sp)
	lw      t0, 36(sp)
	xori    t1, t0, 0
	snez    t0, t1
	sw      t0, 20(sp)
	lw      t0, 20(sp)
	bnez    t0, LLB_2
	j       LLB_3
LLB_2:
	lw      t0, 56(sp)
	lw      t1, 0(t0)
	sw      t1, 24(sp)
	la      t0, fib_arr
	lw      t1, 24(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 12(sp)
	lw      t0, 12(sp)
	lw      t1, 0(t0)
	sw      t1, 8(sp)
	lw      t0, 8(sp)
	mv      a0, t0
	lw      ra, 92(sp)
	addi    sp, sp, 96
	ret     
LLB_3:
	lw      t0, 56(sp)
	lw      t1, 0(t0)
	sw      t1, 48(sp)
	la      t0, fib_arr
	lw      t1, 48(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 64(sp)
	lw      t0, 56(sp)
	lw      t1, 0(t0)
	sw      t1, 68(sp)
	lw      t0, 68(sp)
	li      t1, 1
	sub     t2, t0, t1
	sw      t2, 72(sp)
	lw      t0, 72(sp)
	mv      a0, t0
	call    fib
	sw      a0, 80(sp)
	lw      t0, 56(sp)
	lw      t1, 0(t0)
	sw      t1, 76(sp)
	lw      t0, 76(sp)
	li      t1, 2
	sub     t2, t0, t1
	sw      t2, 60(sp)
	lw      t0, 60(sp)
	mv      a0, t0
	call    fib
	sw      a0, 84(sp)
	lw      t0, 80(sp)
	lw      t1, 84(sp)
	add     t2, t0, t1
	sw      t2, 88(sp)
	lw      t0, 64(sp)
	lw      t1, 88(sp)
	sw      t1, 0(t0)
	lw      t0, 88(sp)
	mv      a0, t0
	lw      ra, 92(sp)
	addi    sp, sp, 96
	ret     
LLB_4:
	li      t0, 0
	mv      a0, t0
	lw      ra, 92(sp)
	addi    sp, sp, 96
	ret     
main:
	addi    sp, sp, -32
	sw      ra, 28(sp)
LLB_6:
	addi    t0, sp, 12
	sw      t0, 8(sp)
	call    getint
	sw      a0, 16(sp)
	lw      t0, 8(sp)
	lw      t1, 16(sp)
	sw      t1, 0(t0)
	lw      t0, 8(sp)
	lw      t1, 0(t0)
	sw      t1, 20(sp)
	lw      t0, 20(sp)
	mv      a0, t0
	call    fib
	sw      a0, 24(sp)
	lw      t0, 24(sp)
	mv      a0, t0
	call    putint
	li      t0, 0
	mv      a0, t0
	lw      ra, 28(sp)
	addi    sp, sp, 32
	ret     
