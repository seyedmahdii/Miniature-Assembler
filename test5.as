	lw	1,0,zero	# $1 = 0
	lw	2,0,ten	# $2 = 10
Loop	slt	3,1,2	# $1 < $2
	beq	3,0,Exit
	addi	1,1,2	# $1 += 2
	addi	2,2,1	# $2 +=1
	j	Loop
Exit	sub	3,1,2
	halt
zero	.fill	0
ten	.fill	10
