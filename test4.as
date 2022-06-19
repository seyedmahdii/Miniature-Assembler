	lw	1,0,d1
start	jalr	2,1
	ori	2,1,3
	nand	2,2,3
	slti	3,2,16
	beq		3,1,start
	lui	3,1
	halt
d1	.fill 	6
