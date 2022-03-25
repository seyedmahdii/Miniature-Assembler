#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 2
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2

struct symbolTable{
	int value;
	char *symbol;
};

struct instruction{
	size_t instType; 	// 0 means R-type, 1 means I-type, 2 means J-type
	size_t intInst;
	char *nemonic;
	char inst[9];		// Ye reshte ke Hex e insuction hast
	int rs;
	int rt;
	int rd;
	int imm;
	int PC;				// Instruction count
};

int findSymbolTableLen(FILE *);
int fillSymbolTable(struct symbolTable *, FILE *);
void formatInstruction(struct instruction *, FILE *);
int hex2Int(char *);
void int2Hex16(char *, int);	// imm value ke 16 bit e