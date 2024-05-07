	.data
	.global d
d:
	.zero 4
	.global temp
temp:
	.zero 8388608
	.global a
a:
	.zero 8388608
	.global b
b:
	.zero 8388608
	.global c
c:
	.zero 8388608
	.text
	.global main
multiply:
	addi    sp, sp, -128
	sw      ra, 124(sp)
LLB_7:
	addi    t0, sp, 24
	sw      t0, 72(sp)
	lw      t0, 72(sp)
	mv      t1, a0
	sw      t1, 0(t0)
	addi    t0, sp, 36
	sw      t0, 64(sp)
	lw      t0, 64(sp)
	mv      t1, a1
	sw      t1, 0(t0)
	lw      t0, 64(sp)
	lw      t1, 0(t0)
	sw      t1, 52(sp)
	lw      t0, 52(sp)
	xori    t1, t0, 0
	seqz    t0, t1
	sw      t0, 44(sp)
	lw      t0, 44(sp)
	bnez    t0, LLB_0
	j       LLB_1
LLB_0:
	li      t0, 0
	mv      a0, t0
	lw      ra, 124(sp)
	addi    sp, sp, 128
	ret     
LLB_1:
	lw      t0, 64(sp)
	lw      t1, 0(t0)
	sw      t1, 32(sp)
	lw      t0, 32(sp)
	xori    t1, t0, 1
	seqz    t0, t1
	sw      t0, 20(sp)
	lw      t0, 20(sp)
	bnez    t0, LLB_2
	j       LLB_3
LLB_2:
	lw      t0, 72(sp)
	lw      t1, 0(t0)
	sw      t1, 16(sp)
	lw      t0, 16(sp)
	li      t1, 998244353
	rem     t2, t0, t1
	sw      t2, 28(sp)
	lw      t0, 28(sp)
	mv      a0, t0
	lw      ra, 124(sp)
	addi    sp, sp, 128
	ret     
LLB_3:
	addi    t0, sp, 8
	sw      t0, 12(sp)
	lw      t0, 72(sp)
	lw      t1, 0(t0)
	sw      t1, 56(sp)
	lw      t0, 64(sp)
	lw      t1, 0(t0)
	sw      t1, 48(sp)
	lw      t0, 48(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 84(sp)
	lw      t0, 56(sp)
	mv      a0, t0
	lw      t0, 84(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 68(sp)
	lw      t0, 12(sp)
	lw      t1, 68(sp)
	sw      t1, 0(t0)
	lw      t0, 12(sp)
	lw      t1, 0(t0)
	sw      t1, 76(sp)
	lw      t0, 12(sp)
	lw      t1, 0(t0)
	sw      t1, 88(sp)
	lw      t0, 76(sp)
	lw      t1, 88(sp)
	add     t2, t0, t1
	sw      t2, 92(sp)
	lw      t0, 92(sp)
	li      t1, 998244353
	rem     t2, t0, t1
	sw      t2, 100(sp)
	lw      t0, 12(sp)
	lw      t1, 100(sp)
	sw      t1, 0(t0)
	lw      t0, 64(sp)
	lw      t1, 0(t0)
	sw      t1, 96(sp)
	lw      t0, 96(sp)
	li      t1, 2
	rem     t2, t0, t1
	sw      t2, 104(sp)
	lw      t0, 104(sp)
	xori    t1, t0, 1
	seqz    t0, t1
	sw      t0, 108(sp)
	lw      t0, 108(sp)
	bnez    t0, LLB_4
	j       LLB_5
LLB_4:
	lw      t0, 12(sp)
	lw      t1, 0(t0)
	sw      t1, 112(sp)
	lw      t0, 72(sp)
	lw      t1, 0(t0)
	sw      t1, 80(sp)
	lw      t0, 112(sp)
	lw      t1, 80(sp)
	add     t2, t0, t1
	sw      t2, 40(sp)
	lw      t0, 40(sp)
	li      t1, 998244353
	rem     t2, t0, t1
	sw      t2, 116(sp)
	lw      t0, 116(sp)
	mv      a0, t0
	lw      ra, 124(sp)
	addi    sp, sp, 128
	ret     
LLB_5:
	lw      t0, 12(sp)
	lw      t1, 0(t0)
	sw      t1, 120(sp)
	lw      t0, 120(sp)
	mv      a0, t0
	lw      ra, 124(sp)
	addi    sp, sp, 128
	ret     
LLB_6:
	li      t0, 0
	mv      a0, t0
	lw      ra, 124(sp)
	addi    sp, sp, 128
	ret     
power:
	addi    sp, sp, -112
	sw      ra, 108(sp)
LLB_13:
	addi    t0, sp, 56
	sw      t0, 76(sp)
	lw      t0, 76(sp)
	mv      t1, a0
	sw      t1, 0(t0)
	addi    t0, sp, 44
	sw      t0, 48(sp)
	lw      t0, 48(sp)
	mv      t1, a1
	sw      t1, 0(t0)
	lw      t0, 48(sp)
	lw      t1, 0(t0)
	sw      t1, 36(sp)
	lw      t0, 36(sp)
	xori    t1, t0, 0
	seqz    t0, t1
	sw      t0, 32(sp)
	lw      t0, 32(sp)
	bnez    t0, LLB_8
	j       LLB_9
LLB_8:
	li      t0, 1
	mv      a0, t0
	lw      ra, 108(sp)
	addi    sp, sp, 112
	ret     
LLB_9:
	addi    t0, sp, 52
	sw      t0, 28(sp)
	lw      t0, 76(sp)
	lw      t1, 0(t0)
	sw      t1, 68(sp)
	lw      t0, 48(sp)
	lw      t1, 0(t0)
	sw      t1, 24(sp)
	lw      t0, 24(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 64(sp)
	lw      t0, 68(sp)
	mv      a0, t0
	lw      t0, 64(sp)
	mv      a1, t0
	call    power
	sw      a0, 20(sp)
	lw      t0, 28(sp)
	lw      t1, 20(sp)
	sw      t1, 0(t0)
	lw      t0, 28(sp)
	lw      t1, 0(t0)
	sw      t1, 80(sp)
	lw      t0, 28(sp)
	lw      t1, 0(t0)
	sw      t1, 72(sp)
	lw      t0, 80(sp)
	mv      a0, t0
	lw      t0, 72(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 84(sp)
	lw      t0, 28(sp)
	lw      t1, 84(sp)
	sw      t1, 0(t0)
	lw      t0, 48(sp)
	lw      t1, 0(t0)
	sw      t1, 88(sp)
	lw      t0, 88(sp)
	li      t1, 2
	rem     t2, t0, t1
	sw      t2, 92(sp)
	lw      t0, 92(sp)
	xori    t1, t0, 1
	seqz    t0, t1
	sw      t0, 96(sp)
	lw      t0, 96(sp)
	bnez    t0, LLB_10
	j       LLB_11
LLB_10:
	lw      t0, 28(sp)
	lw      t1, 0(t0)
	sw      t1, 40(sp)
	lw      t0, 76(sp)
	lw      t1, 0(t0)
	sw      t1, 100(sp)
	lw      t0, 40(sp)
	mv      a0, t0
	lw      t0, 100(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 104(sp)
	lw      t0, 104(sp)
	mv      a0, t0
	lw      ra, 108(sp)
	addi    sp, sp, 112
	ret     
LLB_11:
	lw      t0, 28(sp)
	lw      t1, 0(t0)
	sw      t1, 60(sp)
	lw      t0, 60(sp)
	mv      a0, t0
	lw      ra, 108(sp)
	addi    sp, sp, 112
	ret     
LLB_12:
	li      t0, 0
	mv      a0, t0
	lw      ra, 108(sp)
	addi    sp, sp, 112
	ret     
MemMove:
	addi    sp, sp, -112
LLB_18:
	addi    t0, sp, 60
	sw      t0, 28(sp)
	lw      t0, 28(sp)
	mv      t1, a0
	sw      t1, 0(t0)
	addi    t0, sp, 12
	sw      t0, 48(sp)
	lw      t0, 48(sp)
	mv      t1, a1
	sw      t1, 0(t0)
	addi    t0, sp, 72
	sw      t0, 52(sp)
	lw      t0, 52(sp)
	mv      t1, a2
	sw      t1, 0(t0)
	addi    t0, sp, 36
	sw      t0, 44(sp)
	lw      t0, 44(sp)
	mv      t1, a3
	sw      t1, 0(t0)
	addi    t0, sp, 16
	sw      t0, 32(sp)
	lw      t0, 32(sp)
	li      t1, 0
	sw      t1, 0(t0)
	j       LLB_16
LLB_16:
	lw      t0, 32(sp)
	lw      t1, 0(t0)
	sw      t1, 8(sp)
	lw      t0, 44(sp)
	lw      t1, 0(t0)
	sw      t1, 56(sp)
	lw      t0, 8(sp)
	lw      t1, 56(sp)
	slt     t2, t0, t1
	sw      t2, 24(sp)
	lw      t0, 24(sp)
	bnez    t0, LLB_17
	j       LLB_15
LLB_17:
	lw      t0, 48(sp)
	lw      t1, 0(t0)
	sw      t1, 64(sp)
	lw      t0, 32(sp)
	lw      t1, 0(t0)
	sw      t1, 76(sp)
	lw      t0, 64(sp)
	lw      t1, 76(sp)
	add     t2, t0, t1
	sw      t2, 40(sp)
	lw      t0, 28(sp)
	lw      t1, 0(t0)
	sw      t1, 80(sp)
	lw      t0, 80(sp)
	lw      t1, 40(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 88(sp)
	lw      t0, 32(sp)
	lw      t1, 0(t0)
	sw      t1, 20(sp)
	lw      t0, 52(sp)
	lw      t1, 0(t0)
	sw      t1, 92(sp)
	lw      t0, 92(sp)
	lw      t1, 20(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 96(sp)
	lw      t0, 96(sp)
	lw      t1, 0(t0)
	sw      t1, 100(sp)
	lw      t0, 88(sp)
	lw      t1, 100(sp)
	sw      t1, 0(t0)
	j       LLB_14
LLB_14:
	lw      t0, 32(sp)
	lw      t1, 0(t0)
	sw      t1, 84(sp)
	lw      t0, 84(sp)
	addi    t1, t0, 1
	sw      t1, 104(sp)
	lw      t0, 32(sp)
	lw      t1, 104(sp)
	sw      t1, 0(t0)
	j       LLB_16
LLB_15:
	lw      t0, 44(sp)
	lw      t1, 0(t0)
	sw      t1, 68(sp)
	lw      t0, 68(sp)
	li      t1, 1
	sub     t2, t0, t1
	sw      t2, 108(sp)
	lw      t0, 108(sp)
	mv      a0, t0
	addi    sp, sp, 112
	ret     
fft:
	addi    sp, sp, -544
	sw      ra, 540(sp)
LLB_32:
	addi    t0, sp, 384
	sw      t0, 104(sp)
	lw      t0, 104(sp)
	mv      t1, a0
	sw      t1, 0(t0)
	addi    t0, sp, 252
	sw      t0, 260(sp)
	lw      t0, 260(sp)
	mv      t1, a1
	sw      t1, 0(t0)
	addi    t0, sp, 304
	sw      t0, 416(sp)
	lw      t0, 416(sp)
	mv      t1, a2
	sw      t1, 0(t0)
	addi    t0, sp, 436
	sw      t0, 420(sp)
	lw      t0, 420(sp)
	mv      t1, a3
	sw      t1, 0(t0)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 196(sp)
	lw      t0, 196(sp)
	xori    t1, t0, 1
	seqz    t0, t1
	sw      t0, 412(sp)
	lw      t0, 412(sp)
	bnez    t0, LLB_19
	j       LLB_20
LLB_19:
	li      t0, 1
	mv      a0, t0
	lw      ra, 540(sp)
	addi    sp, sp, 544
	ret     
LLB_20:
	addi    t0, sp, 216
	sw      t0, 428(sp)
	lw      t0, 428(sp)
	li      t1, 0
	sw      t1, 0(t0)
	j       LLB_23
LLB_23:
	lw      t0, 428(sp)
	lw      t1, 0(t0)
	sw      t1, 332(sp)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 308(sp)
	lw      t0, 332(sp)
	lw      t1, 308(sp)
	slt     t2, t0, t1
	sw      t2, 372(sp)
	lw      t0, 372(sp)
	bnez    t0, LLB_24
	j       LLB_22
LLB_24:
	lw      t0, 428(sp)
	lw      t1, 0(t0)
	sw      t1, 328(sp)
	lw      t0, 328(sp)
	li      t1, 2
	rem     t2, t0, t1
	sw      t2, 408(sp)
	lw      t0, 408(sp)
	xori    t1, t0, 0
	seqz    t0, t1
	sw      t0, 336(sp)
	lw      t0, 336(sp)
	bnez    t0, LLB_25
	j       LLB_26
LLB_25:
	lw      t0, 428(sp)
	lw      t1, 0(t0)
	sw      t1, 16(sp)
	lw      t0, 16(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 444(sp)
	la      t0, temp
	lw      t1, 444(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 44(sp)
	lw      t0, 428(sp)
	lw      t1, 0(t0)
	sw      t1, 320(sp)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 344(sp)
	lw      t0, 320(sp)
	lw      t1, 344(sp)
	add     t2, t0, t1
	sw      t2, 292(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 80(sp)
	lw      t0, 80(sp)
	lw      t1, 292(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 312(sp)
	lw      t0, 312(sp)
	lw      t1, 0(t0)
	sw      t1, 288(sp)
	lw      t0, 44(sp)
	lw      t1, 288(sp)
	sw      t1, 0(t0)
	j       LLB_27
LLB_26:
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 392(sp)
	lw      t0, 392(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 448(sp)
	lw      t0, 428(sp)
	lw      t1, 0(t0)
	sw      t1, 352(sp)
	lw      t0, 352(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 324(sp)
	lw      t0, 448(sp)
	lw      t1, 324(sp)
	add     t2, t0, t1
	sw      t2, 456(sp)
	la      t0, temp
	lw      t1, 456(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 472(sp)
	lw      t0, 428(sp)
	lw      t1, 0(t0)
	sw      t1, 488(sp)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 356(sp)
	lw      t0, 488(sp)
	lw      t1, 356(sp)
	add     t2, t0, t1
	sw      t2, 492(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 316(sp)
	lw      t0, 316(sp)
	lw      t1, 492(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 496(sp)
	lw      t0, 496(sp)
	lw      t1, 0(t0)
	sw      t1, 396(sp)
	lw      t0, 472(sp)
	lw      t1, 396(sp)
	sw      t1, 0(t0)
	j       LLB_27
LLB_27:
	lw      t0, 428(sp)
	lw      t1, 0(t0)
	sw      t1, 468(sp)
	lw      t0, 468(sp)
	addi    t1, t0, 1
	sw      t1, 240(sp)
	lw      t0, 428(sp)
	lw      t1, 240(sp)
	sw      t1, 0(t0)
	j       LLB_21
LLB_21:
	lw      t0, 428(sp)
	lw      t1, 0(t0)
	sw      t1, 124(sp)
	lw      t0, 124(sp)
	addi    t1, t0, 1
	sw      t1, 432(sp)
	lw      t0, 428(sp)
	lw      t1, 432(sp)
	sw      t1, 0(t0)
	j       LLB_23
LLB_22:
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 40(sp)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 504(sp)
	la      t0, temp
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 476(sp)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 484(sp)
	lw      t0, 40(sp)
	mv      a0, t0
	lw      t0, 504(sp)
	mv      a1, t0
	lw      t0, 476(sp)
	mv      a2, t0
	lw      t0, 484(sp)
	mv      a3, t0
	call    MemMove
	sw      a0, 280(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 376(sp)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 508(sp)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 512(sp)
	lw      t0, 512(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 364(sp)
	lw      t0, 420(sp)
	lw      t1, 0(t0)
	sw      t1, 360(sp)
	lw      t0, 420(sp)
	lw      t1, 0(t0)
	sw      t1, 516(sp)
	lw      t0, 360(sp)
	mv      a0, t0
	lw      t0, 516(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 340(sp)
	lw      t0, 376(sp)
	mv      a0, t0
	lw      t0, 508(sp)
	mv      a1, t0
	lw      t0, 364(sp)
	mv      a2, t0
	lw      t0, 340(sp)
	mv      a3, t0
	call    fft
	sw      a0, 268(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 256(sp)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 248(sp)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 232(sp)
	lw      t0, 232(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 72(sp)
	lw      t0, 248(sp)
	lw      t1, 72(sp)
	add     t2, t0, t1
	sw      t2, 220(sp)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 212(sp)
	lw      t0, 212(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 204(sp)
	lw      t0, 420(sp)
	lw      t1, 0(t0)
	sw      t1, 200(sp)
	lw      t0, 420(sp)
	lw      t1, 0(t0)
	sw      t1, 188(sp)
	lw      t0, 200(sp)
	mv      a0, t0
	lw      t0, 188(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 440(sp)
	lw      t0, 256(sp)
	mv      a0, t0
	lw      t0, 220(sp)
	mv      a1, t0
	lw      t0, 204(sp)
	mv      a2, t0
	lw      t0, 440(sp)
	mv      a3, t0
	call    fft
	sw      a0, 184(sp)
	addi    t0, sp, 120
	sw      t0, 208(sp)
	lw      t0, 208(sp)
	li      t1, 1
	sw      t1, 0(t0)
	addi    t0, sp, 180
	sw      t0, 500(sp)
	lw      t0, 500(sp)
	li      t1, 0
	sw      t1, 0(t0)
	j       LLB_30
LLB_30:
	lw      t0, 500(sp)
	lw      t1, 0(t0)
	sw      t1, 92(sp)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 404(sp)
	lw      t0, 404(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 168(sp)
	lw      t0, 92(sp)
	lw      t1, 168(sp)
	slt     t2, t0, t1
	sw      t2, 380(sp)
	lw      t0, 380(sp)
	bnez    t0, LLB_31
	j       LLB_29
LLB_31:
	addi    t0, sp, 160
	sw      t0, 152(sp)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 368(sp)
	lw      t0, 500(sp)
	lw      t1, 0(t0)
	sw      t1, 28(sp)
	lw      t0, 368(sp)
	lw      t1, 28(sp)
	add     t2, t0, t1
	sw      t2, 156(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 148(sp)
	lw      t0, 148(sp)
	lw      t1, 156(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 172(sp)
	lw      t0, 172(sp)
	lw      t1, 0(t0)
	sw      t1, 296(sp)
	lw      t0, 152(sp)
	lw      t1, 296(sp)
	sw      t1, 0(t0)
	addi    t0, sp, 264
	sw      t0, 224(sp)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 144(sp)
	lw      t0, 500(sp)
	lw      t1, 0(t0)
	sw      t1, 132(sp)
	lw      t0, 144(sp)
	lw      t1, 132(sp)
	add     t2, t0, t1
	sw      t2, 452(sp)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 136(sp)
	lw      t0, 136(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 128(sp)
	lw      t0, 452(sp)
	lw      t1, 128(sp)
	add     t2, t0, t1
	sw      t2, 244(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 116(sp)
	lw      t0, 116(sp)
	lw      t1, 244(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 112(sp)
	lw      t0, 112(sp)
	lw      t1, 0(t0)
	sw      t1, 108(sp)
	lw      t0, 224(sp)
	lw      t1, 108(sp)
	sw      t1, 0(t0)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 348(sp)
	lw      t0, 500(sp)
	lw      t1, 0(t0)
	sw      t1, 88(sp)
	lw      t0, 348(sp)
	lw      t1, 88(sp)
	add     t2, t0, t1
	sw      t2, 100(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 84(sp)
	lw      t0, 84(sp)
	lw      t1, 100(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 76(sp)
	lw      t0, 152(sp)
	lw      t1, 0(t0)
	sw      t1, 64(sp)
	lw      t0, 208(sp)
	lw      t1, 0(t0)
	sw      t1, 276(sp)
	lw      t0, 224(sp)
	lw      t1, 0(t0)
	sw      t1, 56(sp)
	lw      t0, 276(sp)
	mv      a0, t0
	lw      t0, 56(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 272(sp)
	lw      t0, 64(sp)
	lw      t1, 272(sp)
	add     t2, t0, t1
	sw      t2, 60(sp)
	lw      t0, 60(sp)
	li      t1, 998244353
	rem     t2, t0, t1
	sw      t2, 52(sp)
	lw      t0, 76(sp)
	lw      t1, 52(sp)
	sw      t1, 0(t0)
	lw      t0, 260(sp)
	lw      t1, 0(t0)
	sw      t1, 388(sp)
	lw      t0, 500(sp)
	lw      t1, 0(t0)
	sw      t1, 48(sp)
	lw      t0, 388(sp)
	lw      t1, 48(sp)
	add     t2, t0, t1
	sw      t2, 36(sp)
	lw      t0, 416(sp)
	lw      t1, 0(t0)
	sw      t1, 24(sp)
	lw      t0, 24(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 20(sp)
	lw      t0, 36(sp)
	lw      t1, 20(sp)
	add     t2, t0, t1
	sw      t2, 228(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 12(sp)
	lw      t0, 12(sp)
	lw      t1, 228(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 8(sp)
	lw      t0, 152(sp)
	lw      t1, 0(t0)
	sw      t1, 96(sp)
	lw      t0, 208(sp)
	lw      t1, 0(t0)
	sw      t1, 4(sp)
	lw      t0, 224(sp)
	lw      t1, 0(t0)
	sw      t1, 68(sp)
	lw      t0, 4(sp)
	mv      a0, t0
	lw      t0, 68(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 284(sp)
	lw      t0, 96(sp)
	lw      t1, 284(sp)
	sub     t2, t0, t1
	sw      t2, 400(sp)
	lw      t0, 400(sp)
	li      t2, 998244353
	add     t1, t0, t2
	sw      t1, 140(sp)
	lw      t0, 140(sp)
	li      t1, 998244353
	rem     t2, t0, t1
	sw      t2, 520(sp)
	lw      t0, 8(sp)
	lw      t1, 520(sp)
	sw      t1, 0(t0)
	lw      t0, 208(sp)
	lw      t1, 0(t0)
	sw      t1, 464(sp)
	lw      t0, 420(sp)
	lw      t1, 0(t0)
	sw      t1, 524(sp)
	lw      t0, 464(sp)
	mv      a0, t0
	lw      t0, 524(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 528(sp)
	lw      t0, 208(sp)
	lw      t1, 528(sp)
	sw      t1, 0(t0)
	j       LLB_28
LLB_28:
	lw      t0, 500(sp)
	lw      t1, 0(t0)
	sw      t1, 532(sp)
	lw      t0, 532(sp)
	addi    t1, t0, 1
	sw      t1, 536(sp)
	lw      t0, 500(sp)
	lw      t1, 536(sp)
	sw      t1, 0(t0)
	j       LLB_30
LLB_29:
	li      t0, 0
	mv      a0, t0
	lw      ra, 540(sp)
	addi    sp, sp, 544
	ret     
main:
	addi    sp, sp, -304
	sw      ra, 300(sp)
LLB_44:
	addi    t0, sp, 204
	sw      t0, 212(sp)
	la      t0, a
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 176(sp)
	lw      t0, 176(sp)
	mv      a0, t0
	call    getarray
	sw      a0, 156(sp)
	lw      t0, 212(sp)
	lw      t1, 156(sp)
	sw      t1, 0(t0)
	addi    t0, sp, 148
	sw      t0, 112(sp)
	la      t0, b
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 28(sp)
	lw      t0, 28(sp)
	mv      a0, t0
	call    getarray
	sw      a0, 164(sp)
	lw      t0, 112(sp)
	lw      t1, 164(sp)
	sw      t1, 0(t0)
	call    starttime
	la      t0, d
	li      t1, 1
	sw      t1, 0(t0)
	j       LLB_35
LLB_35:
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 184(sp)
	lw      t0, 212(sp)
	lw      t1, 0(t0)
	sw      t1, 208(sp)
	lw      t0, 112(sp)
	lw      t1, 0(t0)
	sw      t1, 108(sp)
	lw      t0, 208(sp)
	lw      t1, 108(sp)
	add     t2, t0, t1
	sw      t2, 180(sp)
	lw      t0, 180(sp)
	li      t1, 1
	sub     t2, t0, t1
	sw      t2, 92(sp)
	lw      t0, 184(sp)
	lw      t1, 92(sp)
	slt     t2, t0, t1
	sw      t2, 100(sp)
	lw      t0, 100(sp)
	bnez    t0, LLB_36
	j       LLB_34
LLB_36:
	j       LLB_33
LLB_33:
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 216(sp)
	lw      t0, 216(sp)
	li      t1, 2
	mul     t2, t0, t1
	sw      t2, 236(sp)
	la      t0, d
	lw      t1, 236(sp)
	sw      t1, 0(t0)
	j       LLB_35
LLB_34:
	la      t0, a
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 220(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 48(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 120(sp)
	li      t0, 998244352
	lw      t1, 120(sp)
	div     t2, t0, t1
	sw      t2, 228(sp)
	li      t0, 3
	mv      a0, t0
	lw      t0, 228(sp)
	mv      a1, t0
	call    power
	sw      a0, 140(sp)
	lw      t0, 220(sp)
	mv      a0, t0
	li      t0, 0
	mv      a1, t0
	lw      t0, 48(sp)
	mv      a2, t0
	lw      t0, 140(sp)
	mv      a3, t0
	call    fft
	sw      a0, 124(sp)
	la      t0, b
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 192(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 240(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 252(sp)
	li      t0, 998244352
	lw      t1, 252(sp)
	div     t2, t0, t1
	sw      t2, 256(sp)
	li      t0, 3
	mv      a0, t0
	lw      t0, 256(sp)
	mv      a1, t0
	call    power
	sw      a0, 196(sp)
	lw      t0, 192(sp)
	mv      a0, t0
	li      t0, 0
	mv      a1, t0
	lw      t0, 240(sp)
	mv      a2, t0
	lw      t0, 196(sp)
	mv      a3, t0
	call    fft
	sw      a0, 88(sp)
	addi    t0, sp, 80
	sw      t0, 144(sp)
	lw      t0, 144(sp)
	li      t1, 0
	sw      t1, 0(t0)
	j       LLB_37
LLB_37:
	lw      t0, 144(sp)
	lw      t1, 0(t0)
	sw      t1, 188(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 64(sp)
	lw      t0, 188(sp)
	lw      t1, 64(sp)
	slt     t2, t0, t1
	sw      t2, 56(sp)
	lw      t0, 56(sp)
	bnez    t0, LLB_39
	j       LLB_38
LLB_39:
	lw      t0, 144(sp)
	lw      t1, 0(t0)
	sw      t1, 136(sp)
	la      t0, a
	lw      t1, 136(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 132(sp)
	lw      t0, 144(sp)
	lw      t1, 0(t0)
	sw      t1, 44(sp)
	la      t0, a
	lw      t1, 44(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 224(sp)
	lw      t0, 224(sp)
	lw      t1, 0(t0)
	sw      t1, 36(sp)
	lw      t0, 144(sp)
	lw      t1, 0(t0)
	sw      t1, 32(sp)
	la      t0, b
	lw      t1, 32(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 168(sp)
	lw      t0, 168(sp)
	lw      t1, 0(t0)
	sw      t1, 84(sp)
	lw      t0, 36(sp)
	mv      a0, t0
	lw      t0, 84(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 12(sp)
	lw      t0, 132(sp)
	lw      t1, 12(sp)
	sw      t1, 0(t0)
	lw      t0, 144(sp)
	lw      t1, 0(t0)
	sw      t1, 128(sp)
	lw      t0, 128(sp)
	addi    t1, t0, 1
	sw      t1, 152(sp)
	lw      t0, 144(sp)
	lw      t1, 152(sp)
	sw      t1, 0(t0)
	j       LLB_37
LLB_38:
	la      t0, a
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 172(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 8(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 60(sp)
	li      t0, 998244352
	lw      t1, 60(sp)
	div     t2, t0, t1
	sw      t2, 24(sp)
	li      t0, 998244352
	lw      t1, 24(sp)
	sub     t2, t0, t1
	sw      t2, 160(sp)
	li      t0, 3
	mv      a0, t0
	lw      t0, 160(sp)
	mv      a1, t0
	call    power
	sw      a0, 96(sp)
	lw      t0, 172(sp)
	mv      a0, t0
	li      t0, 0
	mv      a1, t0
	lw      t0, 8(sp)
	mv      a2, t0
	lw      t0, 96(sp)
	mv      a3, t0
	call    fft
	sw      a0, 20(sp)
	addi    t0, sp, 4
	sw      t0, 104(sp)
	lw      t0, 104(sp)
	li      t1, 0
	sw      t1, 0(t0)
	j       LLB_42
LLB_42:
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 244(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 52(sp)
	lw      t0, 244(sp)
	lw      t1, 52(sp)
	slt     t2, t0, t1
	sw      t2, 72(sp)
	lw      t0, 72(sp)
	bnez    t0, LLB_43
	j       LLB_41
LLB_43:
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 260(sp)
	la      t0, a
	lw      t1, 260(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 264(sp)
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 16(sp)
	la      t0, a
	lw      t1, 16(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 116(sp)
	lw      t0, 116(sp)
	lw      t1, 0(t0)
	sw      t1, 268(sp)
	la      t0, d
	lw      t1, 0(t0)
	sw      t1, 272(sp)
	lw      t0, 272(sp)
	mv      a0, t0
	li      t0, 998244351
	mv      a1, t0
	call    power
	sw      a0, 276(sp)
	lw      t0, 268(sp)
	mv      a0, t0
	lw      t0, 276(sp)
	mv      a1, t0
	call    multiply
	sw      a0, 284(sp)
	lw      t0, 264(sp)
	lw      t1, 284(sp)
	sw      t1, 0(t0)
	j       LLB_40
LLB_40:
	lw      t0, 104(sp)
	lw      t1, 0(t0)
	sw      t1, 288(sp)
	lw      t0, 288(sp)
	addi    t1, t0, 1
	sw      t1, 76(sp)
	lw      t0, 104(sp)
	lw      t1, 76(sp)
	sw      t1, 0(t0)
	j       LLB_42
LLB_41:
	call    stoptime
	lw      t0, 212(sp)
	lw      t1, 0(t0)
	sw      t1, 292(sp)
	lw      t0, 112(sp)
	lw      t1, 0(t0)
	sw      t1, 68(sp)
	lw      t0, 292(sp)
	lw      t1, 68(sp)
	add     t2, t0, t1
	sw      t2, 296(sp)
	lw      t0, 296(sp)
	li      t1, 1
	sub     t2, t0, t1
	sw      t2, 200(sp)
	la      t0, a
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 280(sp)
	lw      t0, 200(sp)
	mv      a0, t0
	lw      t0, 280(sp)
	mv      a1, t0
	call    putarray
	li      t0, 0
	mv      a0, t0
	lw      ra, 300(sp)
	addi    sp, sp, 304
	ret     
