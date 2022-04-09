#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

struct symbolTable{
	int value;
	char *symbol;
};

struct instruction{
	char instType; 	// 0 for R-type, 1 for I-type, 2 for J-type
	char *mnemonic;
	char instBin[33];
	long long instDec;
	int opCode;
	int rs;
	int rt;
	int rd;
	int imm;
	int PC;
};

int findSymbolTableLen(FILE *);
int fillSymbolTable(struct symbolTable *, FILE *);
void formInstruction(struct instruction *);
int getLableValue(struct symbolTable *, int , char *);
bool isLable(char *);
void writeToFile(FILE *, int);
char *getNthLine(FILE *, int);
long long int2Binary(int dec);	// Converts decimal to binary
char *binaryExtend(long long bin, int len, char sign);	// Etxends binary
long long bin2Dec(char *, int);
void checkDuplicateLabels(struct symbolTable *, int);
bool isOffsetAcceptable(int);
void tooLargeOffsetErrorHandler(int offset, int lineNo);
void undefinedLabelErrorHandler(char *label, int lineNo);
long long complement2s(long long bin);