	lw	1,0,d1
	lw	2,0,d2
	addi	1,1,-3
	slt	3,1,2
	beq	3,0,done
	sub	2,2,1
	sw	2,0,8
done	halt
d1	.fill	5	
d2	.fill	12
