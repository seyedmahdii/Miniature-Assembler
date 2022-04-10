	addi	1,0,0
	lw	2,0,num
Loop	slti	3,1,100	# $1 < 100
	beq	3,0,Exit
	addi	1,1,2	# $1 += $2
	j	Loop
Exit	or	3,1,2
	nand	3,3,2
	ori	3,3,5
	halt	# halt the program
num	.fill	4	
