	lw	1,0,lbl1	
	lw	2,0,lbl # Error: Undefined label
	sub	1,1,2
	halt
lbl1	.fill 	4
lbl2	.fill	5
