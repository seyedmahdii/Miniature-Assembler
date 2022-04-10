	lw	1,0,d1
start	jalr	2,mul16	# call mul16
	ori	2,1,3
	nand	2,2,3
	slti	3,2,16
	beq		3,1,start
	halt
mul16	lui	3,1
d1	.fill 	5
