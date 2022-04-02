#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
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
	char instType; 	// 0 means R-type, 1 means I-type, 2 means J-type
	size_t intInst;		// Aval instruction ro mibarim be hex va baad tabdil be decimal
	char *mnemonic;
	char instBin[33];
	long long instDec;
	int opCode;
	int rs;
	int rt;
	int rd;
	int imm;
	int PC;				// Instruction count YA shomare dast amal
};

struct memoryTable{
	char lable[7];
	int value;
	int address;
};

int findSymbolTableLen(FILE *);
int fillSymbolTable(struct symbolTable *, FILE *);
void formInstruction(struct instruction *);
int hex2Int(char *);
void int2Hex16(char *, int);	// imm value ke 16 bit e
int getLableValue(struct symbolTable *, int , char *);
bool isLable(char *);
void writeToFile(FILE *, int);
int getLableAdress(struct memoryTable *, char *);	// Returns the address of lable
int getAdressValue(struct memoryTable *, int);	// Returns the value of a address
bool setAdressValue(struct memoryTable *, int address, int value);	// Sets the value of a address
char *getNthLine(FILE *, int);
long long int2Binary(int dec);	// Converts decimal to binary
char *binaryExtend(long long bin, int len, char sign);	// Etxends binary
long long bin2Dec(char *, int);
void goToNthLine(FILE *, int);