	.data
	.global buf
buf:
	.zero 800
	.text
	.global main
merge_sort:
	addi    sp, sp, -448
	sw      ra, 444(sp)
LLB_17:
	addi    t0, sp, 228
	sw      t0, 128(sp)
	lw      t0, 128(sp)
	mv      t1, a0
	sw      t1, 0(t0)
	addi    t0, sp, 204
	sw      t0, 212(sp)
	lw      t0, 212(sp)
	mv      t1, a1
	sw      t1, 0(t0)
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 196(sp)
	lw      t0, 196(sp)
	addi    t1, t0, 1
	sw      t1, 184(sp)
	lw      t0, 212(sp)
	lw      t1, 0(t0)
	sw      t1, 176(sp)
	lw      t0, 184(sp)
	lw      t1, 176(sp)
	slt     t2, t0, t1
	xori    t0, t2, 1
	sw      t0, 272(sp)
	lw      t0, 272(sp)
	bnez    t0, LLB_0
	j       LLB_1
LLB_0:
	lw      ra, 444(sp)
	addi    sp, sp, 448
	ret     
LLB_1:
	addi    t0, sp, 172
	sw      t0, 248(sp)
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 108(sp)
	lw      t0, 212(sp)
	lw      t1, 0(t0)
	sw      t1, 288(sp)
	lw      t0, 108(sp)
	lw      t1, 288(sp)
	add     t2, t0, t1
	sw      t2, 236(sp)
	lw      t0, 236(sp)
	li      t1, 2
	div     t2, t0, t1
	sw      t2, 224(sp)
	lw      t0, 248(sp)
	lw      t1, 224(sp)
	sw      t1, 0(t0)
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 60(sp)
	lw      t0, 248(sp)
	lw      t1, 0(t0)
	sw      t1, 256(sp)
	lw      t0, 60(sp)
	mv      a0, t0
	lw      t0, 256(sp)
	mv      a1, t0
	call    merge_sort
	lw      t0, 248(sp)
	lw      t1, 0(t0)
	sw      t1, 44(sp)
	lw      t0, 212(sp)
	lw      t1, 0(t0)
	sw      t1, 268(sp)
	lw      t0, 44(sp)
	mv      a0, t0
	lw      t0, 268(sp)
	mv      a1, t0
	call    merge_sort
	addi    t0, sp, 208
	sw      t0, 280(sp)
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 276(sp)
	lw      t0, 280(sp)
	lw      t1, 276(sp)
	sw      t1, 0(t0)
	addi    t0, sp, 292
	sw      t0, 220(sp)
	lw      t0, 248(sp)
	lw      t1, 0(t0)
	sw      t1, 296(sp)
	lw      t0, 220(sp)
	lw      t1, 296(sp)
	sw      t1, 0(t0)
	addi    t0, sp, 36
	sw      t0, 304(sp)
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 168(sp)
	lw      t0, 304(sp)
	lw      t1, 168(sp)
	sw      t1, 0(t0)
	j       LLB_2
LLB_2:
	lw      t0, 280(sp)
	lw      t1, 0(t0)
	sw      t1, 160(sp)
	lw      t0, 248(sp)
	lw      t1, 0(t0)
	sw      t1, 232(sp)
	lw      t0, 160(sp)
	lw      t1, 232(sp)
	slt     t2, t0, t1
	sw      t2, 136(sp)
	lw      t0, 136(sp)
	xori    t1, t0, 0
	snez    t0, t1
	sw      t0, 152(sp)
	lw      t0, 220(sp)
	lw      t1, 0(t0)
	sw      t1, 132(sp)
	lw      t0, 212(sp)
	lw      t1, 0(t0)
	sw      t1, 164(sp)
	lw      t0, 132(sp)
	lw      t1, 164(sp)
	slt     t2, t0, t1
	sw      t2, 120(sp)
	lw      t0, 120(sp)
	xori    t1, t0, 0
	snez    t0, t1
	sw      t0, 264(sp)
	lw      t0, 152(sp)
	lw      t1, 264(sp)
	and     t2, t0, t1
	sw      t2, 112(sp)
	lw      t0, 112(sp)
	bnez    t0, LLB_4
	j       LLB_3
LLB_4:
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 100(sp)
	lw      t0, 280(sp)
	lw      t1, 0(t0)
	sw      t1, 92(sp)
	lw      t0, 100(sp)
	lw      t1, 92(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 124(sp)
	lw      t0, 124(sp)
	lw      t1, 0(t0)
	sw      t1, 80(sp)
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 240(sp)
	lw      t0, 220(sp)
	lw      t1, 0(t0)
	sw      t1, 76(sp)
	lw      t0, 240(sp)
	lw      t1, 76(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 52(sp)
	lw      t0, 52(sp)
	lw      t1, 0(t0)
	sw      t1, 96(sp)
	lw      t0, 80(sp)
	lw      t1, 96(sp)
	slt     t2, t0, t1
	sw      t2, 180(sp)
	lw      t0, 180(sp)
	bnez    t0, LLB_5
	j       LLB_6
LLB_5:
	la      t0, buf
	li      t1, 1
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 64(sp)
	lw      t0, 304(sp)
	lw      t1, 0(t0)
	sw      t1, 104(sp)
	lw      t0, 64(sp)
	lw      t1, 104(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 40(sp)
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 300(sp)
	lw      t0, 280(sp)
	lw      t1, 0(t0)
	sw      t1, 24(sp)
	lw      t0, 300(sp)
	lw      t1, 24(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 16(sp)
	lw      t0, 16(sp)
	lw      t1, 0(t0)
	sw      t1, 144(sp)
	lw      t0, 40(sp)
	lw      t1, 144(sp)
	sw      t1, 0(t0)
	lw      t0, 280(sp)
	lw      t1, 0(t0)
	sw      t1, 28(sp)
	lw      t0, 28(sp)
	addi    t1, t0, 1
	sw      t1, 56(sp)
	lw      t0, 280(sp)
	lw      t1, 56(sp)
	sw      t1, 0(t0)
	j       LLB_7
LLB_6:
	la      t0, buf
	li      t1, 1
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 192(sp)
	lw      t0, 304(sp)
	lw      t1, 0(t0)
	sw      t1, 12(sp)
	lw      t0, 192(sp)
	lw      t1, 12(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 308(sp)
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 20(sp)
	lw      t0, 220(sp)
	lw      t1, 0(t0)
	sw      t1, 312(sp)
	lw      t0, 20(sp)
	lw      t1, 312(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 84(sp)
	lw      t0, 84(sp)
	lw      t1, 0(t0)
	sw      t1, 316(sp)
	lw      t0, 308(sp)
	lw      t1, 316(sp)
	sw      t1, 0(t0)
	lw      t0, 220(sp)
	lw      t1, 0(t0)
	sw      t1, 244(sp)
	lw      t0, 244(sp)
	addi    t1, t0, 1
	sw      t1, 332(sp)
	lw      t0, 220(sp)
	lw      t1, 332(sp)
	sw      t1, 0(t0)
	j       LLB_7
LLB_7:
	lw      t0, 304(sp)
	lw      t1, 0(t0)
	sw      t1, 336(sp)
	lw      t0, 336(sp)
	addi    t1, t0, 1
	sw      t1, 188(sp)
	lw      t0, 304(sp)
	lw      t1, 188(sp)
	sw      t1, 0(t0)
	j       LLB_2
LLB_3:
	j       LLB_8
LLB_8:
	lw      t0, 280(sp)
	lw      t1, 0(t0)
	sw      t1, 140(sp)
	lw      t0, 248(sp)
	lw      t1, 0(t0)
	sw      t1, 340(sp)
	lw      t0, 140(sp)
	lw      t1, 340(sp)
	slt     t2, t0, t1
	sw      t2, 200(sp)
	lw      t0, 200(sp)
	bnez    t0, LLB_10
	j       LLB_9
LLB_10:
	la      t0, buf
	li      t1, 1
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 348(sp)
	lw      t0, 304(sp)
	lw      t1, 0(t0)
	sw      t1, 352(sp)
	lw      t0, 348(sp)
	lw      t1, 352(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 356(sp)
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 344(sp)
	lw      t0, 280(sp)
	lw      t1, 0(t0)
	sw      t1, 360(sp)
	lw      t0, 344(sp)
	lw      t1, 360(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 320(sp)
	lw      t0, 320(sp)
	lw      t1, 0(t0)
	sw      t1, 48(sp)
	lw      t0, 356(sp)
	lw      t1, 48(sp)
	sw      t1, 0(t0)
	lw      t0, 280(sp)
	lw      t1, 0(t0)
	sw      t1, 364(sp)
	lw      t0, 364(sp)
	addi    t1, t0, 1
	sw      t1, 372(sp)
	lw      t0, 280(sp)
	lw      t1, 372(sp)
	sw      t1, 0(t0)
	lw      t0, 304(sp)
	lw      t1, 0(t0)
	sw      t1, 116(sp)
	lw      t0, 116(sp)
	addi    t1, t0, 1
	sw      t1, 380(sp)
	lw      t0, 304(sp)
	lw      t1, 380(sp)
	sw      t1, 0(t0)
	j       LLB_8
LLB_9:
	j       LLB_11
LLB_11:
	lw      t0, 220(sp)
	lw      t1, 0(t0)
	sw      t1, 384(sp)
	lw      t0, 212(sp)
	lw      t1, 0(t0)
	sw      t1, 388(sp)
	lw      t0, 384(sp)
	lw      t1, 388(sp)
	slt     t2, t0, t1
	sw      t2, 400(sp)
	lw      t0, 400(sp)
	bnez    t0, LLB_13
	j       LLB_12
LLB_13:
	la      t0, buf
	li      t1, 1
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 68(sp)
	lw      t0, 304(sp)
	lw      t1, 0(t0)
	sw      t1, 72(sp)
	lw      t0, 68(sp)
	lw      t1, 72(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 392(sp)
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 404(sp)
	lw      t0, 220(sp)
	lw      t1, 0(t0)
	sw      t1, 368(sp)
	lw      t0, 404(sp)
	lw      t1, 368(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 408(sp)
	lw      t0, 408(sp)
	lw      t1, 0(t0)
	sw      t1, 252(sp)
	lw      t0, 392(sp)
	lw      t1, 252(sp)
	sw      t1, 0(t0)
	lw      t0, 220(sp)
	lw      t1, 0(t0)
	sw      t1, 284(sp)
	lw      t0, 284(sp)
	addi    t1, t0, 1
	sw      t1, 412(sp)
	lw      t0, 220(sp)
	lw      t1, 412(sp)
	sw      t1, 0(t0)
	lw      t0, 304(sp)
	lw      t1, 0(t0)
	sw      t1, 324(sp)
	lw      t0, 324(sp)
	addi    t1, t0, 1
	sw      t1, 376(sp)
	lw      t0, 304(sp)
	lw      t1, 376(sp)
	sw      t1, 0(t0)
	j       LLB_11
LLB_12:
	j       LLB_14
LLB_14:
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 416(sp)
	lw      t0, 212(sp)
	lw      t1, 0(t0)
	sw      t1, 88(sp)
	lw      t0, 416(sp)
	lw      t1, 88(sp)
	slt     t2, t0, t1
	sw      t2, 420(sp)
	lw      t0, 420(sp)
	bnez    t0, LLB_16
	j       LLB_15
LLB_16:
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 396(sp)
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 424(sp)
	lw      t0, 396(sp)
	lw      t1, 424(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 260(sp)
	la      t0, buf
	li      t1, 1
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 428(sp)
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 432(sp)
	lw      t0, 428(sp)
	lw      t1, 432(sp)
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 216(sp)
	lw      t0, 216(sp)
	lw      t1, 0(t0)
	sw      t1, 436(sp)
	lw      t0, 260(sp)
	lw      t1, 436(sp)
	sw      t1, 0(t0)
	lw      t0, 128(sp)
	lw      t1, 0(t0)
	sw      t1, 440(sp)
	lw      t0, 440(sp)
	addi    t1, t0, 1
	sw      t1, 328(sp)
	lw      t0, 128(sp)
	lw      t1, 328(sp)
	sw      t1, 0(t0)
	j       LLB_14
LLB_15:
	lw      ra, 444(sp)
	addi    sp, sp, 448
	ret     
main:
	addi    sp, sp, -48
	sw      ra, 44(sp)
LLB_18:
	addi    t0, sp, 20
	sw      t0, 8(sp)
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 24(sp)
	lw      t0, 24(sp)
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 28(sp)
	lw      t0, 28(sp)
	mv      a0, t0
	call    getarray
	sw      a0, 12(sp)
	lw      t0, 8(sp)
	lw      t1, 12(sp)
	sw      t1, 0(t0)
	lw      t0, 8(sp)
	lw      t1, 0(t0)
	sw      t1, 32(sp)
	li      t0, 0
	mv      a0, t0
	lw      t0, 32(sp)
	mv      a1, t0
	call    merge_sort
	lw      t0, 8(sp)
	lw      t1, 0(t0)
	sw      t1, 36(sp)
	la      t0, buf
	li      t1, 0
	li      t2, 400
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 40(sp)
	lw      t0, 40(sp)
	li      t1, 0
	li      t2, 4
	mul     t1, t1, t2
	add     t0, t0, t1
	sw      t0, 16(sp)
	lw      t0, 36(sp)
	mv      a0, t0
	lw      t0, 16(sp)
	mv      a1, t0
	call    putarray
	li      t0, 0
	mv      a0, t0
	lw      ra, 44(sp)
	addi    sp, sp, 48
	ret     
