#include "assemble.h"
const int memorySize = 8192;

void main(int argc,char **argv){
   FILE *assp, *machp, *fopen();
   struct symbolTable *pSymbolTable;
   int symbolTableLen;
   int registers[16];
   registers[0] = 0;
   struct memoryTable *pMemoryTable;
   int memoryCounter = 0;
   int i, j, noInsts;
   struct instruction *currInst;
   size_t lineSize;
   char *line;
   char *token;
   char lable[7];
   char *instructions[] = {
      "add", "sub", "slt", "or", "nand",
      "addi", "slti", "ori", "lui", "lw", "sw", "beq", "jalr",
      "j", "halt"
   };
   int instCount = 0;

   i = 0;
   j = 0;
   line = (char *)malloc(72);
   currInst = (struct instruction *)malloc(sizeof(struct instruction));

   if(argc < 3){
      printf("***** Please run this program as follows:\n");
      printf("***** %s assprog.as machprog.m\n",argv[0]);
      printf("***** where assprog.as is your assembly program\n");
      printf("***** and machprog.m will be your machine code.\n");
      exit(1);
   }
   if((assp=fopen(argv[1],"r")) == NULL){
      printf("%s cannot be openned\n",argv[1]);
      exit(1);
   }
   if((machp=fopen(argv[2],"w+")) == NULL){
      printf("%s cannot be openned\n",argv[2]);
      exit(1);
   }
   // here you can place your code for the assembler

   // Constructing symbol table
   symbolTableLen = findSymbolTableLen(assp);
   pSymbolTable = (struct symbolTable *)malloc(symbolTableLen*sizeof(struct symbolTable));
   for(i=0; i<symbolTableLen; i++){
      pSymbolTable[i].symbol = (char *)malloc(7);
   }
   noInsts = fillSymbolTable(pSymbolTable, assp);

   // Initializing memory table
   pMemoryTable = (struct memoryTable *)malloc(memorySize*sizeof(struct memoryTable));

   while(getline(&line, &lineSize, assp) != -1){
      currInst->PC = instCount;
      instCount++;
      token = strtok(line, "\t, \n");
      strcpy(lable, token);   // Saving the first word for .fill
      for(i=0; i<symbolTableLen; i++){
         // avalin token == sybmolTable[i].symbol, agar yeki bod, miram sare token baadi. token badi 
         // mishe instruction am. age nabod hamon token instruction man hast.
         if(strcmp(pSymbolTable[i].symbol, token) == 0){
            token = strtok(NULL, "\t, \n");
         }
      }

      strcpy(currInst->mnemonic, token);
      currInst->instType = -1;

      if(strcmp(currInst->mnemonic, ".fill") == 0){
         token = strtok(NULL, "\t, \n");
         if(isLable(token)){
            currInst->intInst = getLableValue(pSymbolTable, symbolTableLen, token);
         }
         else{
            currInst->intInst = atoi(token);
         }

         if(memoryCounter < 2048){
            strcpy(pMemoryTable[memoryCounter].lable, lable);
            pMemoryTable[memoryCounter].address = instCount-1;
            pMemoryTable[memoryCounter].value = currInst->intInst;
            writeToFile(machp, pMemoryTable[memoryCounter].value);
            memoryCounter++;
         }
         else{
            // *************
            // Throw exception
         }
      }

      if(strcmp(currInst->mnemonic, "lw") == 0){
         currInst->instType = 1;
         currInst->opCode = 9;
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         if(isLable(token)){
            currInst->imm = getLableValue(pSymbolTable, symbolTableLen, token);  
         }
         else{
            currInst->imm = atoi(token);
         }
         int address = registers[currInst->rs] + currInst->imm;
         // Geting the value of lw address
         FILE *inputFile = fopen(argv[1],"r");
         char *targetLine = getNthLine(inputFile, address+1);
         fclose(inputFile);
         targetLine = strtok(targetLine, "\t, \n");
         targetLine = strtok(NULL, "\t, \n");
         targetLine = strtok(NULL, "\t, \n");
         registers[currInst->rt] = atoi(targetLine);

         // Constructing result in binary
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));

         printf("R[%d]: %d \n", currInst->rt, registers[currInst->rt]);
         printf("rt: %d /t rs: %d \t imm: %d \n", currInst->rt, currInst->rs, currInst->imm);
      }

      if(strcmp(currInst->mnemonic, "add") == 0){
         currInst->instType = 0;
         token = strtok(NULL, "\t, \n");
         currInst->rd = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         // strcpy(currInst->opCode, "0000");
         registers[currInst->rd] = registers[currInst->rs] + registers[currInst->rt];
      }

      if(strcmp(currInst->mnemonic, "sub") == 0){
         currInst->instType = 0;
         token = strtok(NULL, "\t, \n");
         currInst->rd = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         // strcpy(currInst->opCode, "0001");
         registers[currInst->rd] = registers[currInst->rs] - registers[currInst->rt];
      }
   }

   fclose(assp);
   fclose(machp);
}

int findSymbolTableLen(FILE *inputFile){
   int count = 0;
   size_t lineSize;
   char *line;
   line = (char *)malloc(72);
   while(getline(&line, &lineSize, inputFile) != -1){
      if((line[0] == ' ') || (line[0] == '\t'));
      else{
         count++;
      }
   }
   rewind(inputFile);
   free(line);
   return count;
}

int fillSymbolTable(struct symbolTable *symTable, FILE *inputFile){
   int lineNo = 0;
   size_t lineSize;
   char *line;
   int i = 0;
   char *token;
   line = (char *)malloc(72);

   while(getline(&line, &lineSize, inputFile) != -1){
      if((line[0] == ' ') || (line[0] == '\t'));
      else{
         token = strtok(line, "\t, ");
         strcpy(symTable[i].symbol, token);
         symTable[i].value = lineNo;
         i++;
      }  
      lineNo++;
   }
   rewind(inputFile);
   free(line);
   return lineNo;
}

void formInstruction(struct instruction *currInst){
   char temp[33];
   currInst->instBin[0] = currInst->instBin[1] = currInst->instBin[2] = currInst->instBin[3] = '0'; // unused
   currInst->instBin[4] = '\0';

   // opcode
   long long bin = int2Binary(currInst->opCode);
   strcpy(temp, binaryExtend(bin, 4, '0'));
   strcat(currInst->instBin, temp);

   if(currInst->instType == 0){
      
   }
   else if(currInst->instType == 1){
      bin = int2Binary(currInst->rs);
      strcpy(temp, binaryExtend(bin, 4, '0'));
      strcat(currInst->instBin, temp);

      bin = int2Binary(currInst->rt);
      strcpy(temp, binaryExtend(bin, 4, '0'));
      strcat(currInst->instBin, temp);

      bin = int2Binary(currInst->imm);
      strcpy(temp, binaryExtend(bin, 16, '0'));
      strcat(currInst->instBin, temp);
   }
   else if(currInst->instType == 2){
      
   }
}

int hex2Int(char *hex){
   int result = 0;
   while((*hex) != '\0'){
      if((*hex) >= '0' && (*hex) <= '9'){
         result = result * 16 + (*hex) - '0';
      }
      else if((*hex) >= 'a' && (*hex) <= 'f'){
         result = result * 16 + (*hex) - 'a' + 10;
      }
      else if((*hex) >= 'A' && (*hex) <= 'F'){
         result = result * 16 + (*hex) - 'A' + 10;
      }
      hex++;
   }
   return result;
}

void int2Hex16(char *lower, int a){
   sprintf(lower, "%x", a);
   if(a < 0x10){
      lower[4] = '\0';
      lower[3] = lower[0];
      lower[2] = '0';
      lower[1] = '0';
      lower[0] = '0';
   }
   else if(a < 0x100){
      lower[4] = '\0';
      lower[3] = lower[1];
      lower[2] = lower[0];
      lower[1] = '0';
      lower[0] = '0';  
   }
   else if(a < 0x1000){
      lower[4] = '\0';
      lower[3] = lower[2];
      lower[2] = lower[1];
      lower[1] = lower[0];
      lower[0] = '0';  
   }
}

int getLableValue(struct symbolTable *symTable,int symTableLen, char *lable){
   for(int i=0; i<symTableLen; i++){
      if(strcmp(symTable[i].symbol, lable) == 0){
         return symTable[i].value;
      }
   }
   // No such a lable
   return -1;
}

bool isLable(char *str){
   int ascii = str[0];
   if((ascii >= 65 && ascii <= 90) || (ascii >= 97 && ascii <= 122)){
      return true;
   }
   return false;
}

void writeToFile(FILE *outputFile, int value){
   fprintf(outputFile, "%d\n", value);
}

int getLableAdress(struct memoryTable *pMemoryTable, char *lable){
   for(int i=0; i<memorySize; i++){
      if(strcmp(pMemoryTable[i].lable, lable) == 0){
         return pMemoryTable[i].address;
      }
   }
   // No such a lable
   return -1;
}

int getAdressValue(struct memoryTable *pMemoryTable, int address){
   for(int i=0; i<memorySize; i++){
      if(pMemoryTable[i].address == address){
         return pMemoryTable[i].value;
      }
   }
   // No such a lable
   return -1;
}

char *getNthLine(FILE *inputFile, int n){
   int count = 0;
   size_t lineSize;
   char *line;
   line = (char *)malloc(72);
   while(getline(&line, &lineSize, inputFile) != -1){
      count++;
      if(count == n){
         break;
      }
   }
   return (char *)line;
}

long long int2Binary(int num){
   long long bin = 0;
   int rem, i = 1;

   while (num != 0){
      rem = num % 2;
      num /= 2;
      bin += rem * i;
      i *= 10;
   }
   return bin;
}

char *binaryExtend(long long bin, int len, char sign){
   char cBin[33], *final;
   final = (char *)malloc(33);
   sprintf(cBin, "%d", bin);

   int diff = len - strlen(cBin);
   for(int i=0; i<diff; i++){
      final[i] = sign;
   }
   final[diff] = '\0';  // End of character array
   strcat(final, cBin);
   return (char *)final;
}

long long bin2Dec(char *binary, int len){
   long long decimal = 0;
   int position = 0;
   int i = len - 1;
   while(i >= 0){
      decimal = decimal + (binary[i] - 48) * pow(2, position);
      i--;
      position++;
   }
   return decimal;
}