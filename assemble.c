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

   // Error: Duplicate label
   checkDuplicateLabels(pSymbolTable, symbolTableLen);

   // Initializing memory table
   pMemoryTable = (struct memoryTable *)malloc(memorySize*sizeof(struct memoryTable));

   while(getline(&line, &lineSize, assp) != -1){
      currInst->PC = instCount;
      instCount++;
      token = strtok(line, "\t, \n");
      strcpy(lable, token);   // Saving the first word for .fill
      for(i=0; i<symbolTableLen; i++){
         if(strcmp(pSymbolTable[i].symbol, token) == 0){
            token = strtok(NULL, "\t, \n");
         }
      }

      strcpy(currInst->mnemonic, token);
      currInst->instType = -1;

      if(strcmp(currInst->mnemonic, ".fill") == 0){
         token = strtok(NULL, "\t, \n");
         int labelValue;
         if(isLable(token)){
            labelValue = getLableValue(pSymbolTable, symbolTableLen, token);  
            // Error: No such a label in the symbolTable
            if(labelValue == -1){
               undefinedLabelErrorHandler(token, instCount);
            }
         }
         else{
            labelValue = atoi(token);
         }

         // Error: Too large offset
         if(!isOffsetAcceptable(labelValue)){
            tooLargeOffsetErrorHandler(currInst->imm, instCount);
         }

         if(memoryCounter < 2048){
            strcpy(pMemoryTable[memoryCounter].lable, lable);
            pMemoryTable[memoryCounter].address = instCount-1;
            pMemoryTable[memoryCounter].value = labelValue;
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
            // Error: No such a label in the symbolTable
            if(currInst->imm == -1){
               undefinedLabelErrorHandler(token, instCount);
            }
         }
         else{
            currInst->imm = atoi(token);
         }

         // Error: Too large offset
         if(!isOffsetAcceptable(currInst->imm)){
            tooLargeOffsetErrorHandler(currInst->imm, instCount);
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

         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));
      }

      if(strcmp(currInst->mnemonic, "add") == 0){
         currInst->instType = 0;
         currInst->opCode = 0;
         token = strtok(NULL, "\t, \n");
         currInst->rd = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         registers[currInst->rd] = registers[currInst->rs] + registers[currInst->rt];
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "sub") == 0){
         currInst->instType = 0;
         currInst->opCode = 1;
         token = strtok(NULL, "\t, \n");
         currInst->rd = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         registers[currInst->rd] = registers[currInst->rs] - registers[currInst->rt];
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "slt") == 0){
         currInst->instType = 0;
         currInst->opCode = 2;
         token = strtok(NULL, "\t, \n");
         currInst->rd = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         if(registers[currInst->rs] < registers[currInst->rt]){
            registers[currInst->rd] = 1;
         } 
         else{
            registers[currInst->rd] = 0;
         }
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "or") == 0){
         currInst->instType = 0;
         currInst->opCode = 3;
         token = strtok(NULL, "\t, \n");
         currInst->rd = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);

         char or1[33], or2[33], res[33];
         long long bin = int2Binary(registers[currInst->rs]);
         strcpy(or1, binaryExtend(bin, 32, '0'));
         bin = int2Binary(registers[currInst->rt]);
         strcpy(or2, binaryExtend(bin, 32, '0'));
         for(i=0; i<32; i++){
            if(or1[i] == '1' || or2[i] == '1'){
               res[i] = '1';
            }
            else{
               res[i] = '0';  
            }
         }
         registers[currInst->rd] = bin2Dec(res, 32);
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "nand") == 0){
         currInst->instType = 0;
         currInst->opCode = 4;
         token = strtok(NULL, "\t, \n");
         currInst->rd = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);

         char nand1[33], nand2[33], res[33];
         long long bin = int2Binary(registers[currInst->rs]);
         strcpy(nand1, binaryExtend(bin, 32, '0'));
         bin = int2Binary(registers[currInst->rt]);
         strcpy(nand2, binaryExtend(bin, 32, '0'));
         for(i=0; i<32; i++){
            if(nand1[i] == '0' || nand2[i] == '0'){
               res[i] = '1';
            }
            else{
               res[i] = '0';  
            }
         }
         registers[currInst->rd] = bin2Dec(res, 32);
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "addi") == 0){
         currInst->instType = 1;
         currInst->opCode = 5;
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->imm = atoi(token);
         if(!isOffsetAcceptable(currInst->imm)){
            tooLargeOffsetErrorHandler(currInst->imm, instCount);
         }
         registers[currInst->rt] = registers[currInst->rs] + currInst->imm;
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "slti") == 0){
         currInst->instType = 1;
         currInst->opCode = 6;
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->imm = atoi(token);
         if(!isOffsetAcceptable(currInst->imm)){
            tooLargeOffsetErrorHandler(currInst->imm, instCount);
         }
         if(registers[currInst->rs] < currInst->imm){
            registers[currInst->rt] = 1;
         } 
         else{
            registers[currInst->rt] = 0;
         }
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "ori") == 0){
         currInst->instType = 1;
         currInst->opCode = 7;
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->imm = atoi(token);
         if(!isOffsetAcceptable(currInst->imm)){
            tooLargeOffsetErrorHandler(currInst->imm, instCount);
         }
         if(registers[currInst->rs] < currInst->imm){
            registers[currInst->rt] = 1;
         } 

         char or1[33], or2[33], res[33];
         long long bin = int2Binary(registers[currInst->rs]);
         strcpy(or1, binaryExtend(bin, 32, '0'));
         bin = int2Binary(currInst->imm);
         strcpy(or2, binaryExtend(bin, 32, '0'));
         // The first 16 bits of ori result should be zero
         for(i=0; i<16; i++){
            res[i] = '0';
         }
         for(i=16; i<32; i++){
            if(or1[i] == '1' || or2[i] == '1'){
               res[i] = '1';
            }
            else{
               res[i] = '0';  
            }
         }
         registers[currInst->rt] = bin2Dec(res, 32);
         
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "lui") == 0){
         currInst->instType = 1;
         currInst->opCode = 8;
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         currInst->rs = 0;    // rs for lui instruction is 0
         token = strtok(NULL, "\t, \n");
         currInst->imm = atoi(token);
         if(!isOffsetAcceptable(currInst->imm)){
            tooLargeOffsetErrorHandler(currInst->imm, instCount);
         }
         
         registers[currInst->rt] = currInst->imm * pow(2, 16);
         
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));  
      }

      if(strcmp(currInst->mnemonic, "sw") == 0){
         currInst->instType = 1;
         currInst->opCode = 10;
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         if(isLable(token)){
            currInst->imm = getLableValue(pSymbolTable, symbolTableLen, token);  
            // Error: No such a label in the symbolTable
            if(currInst->imm == -1){
               undefinedLabelErrorHandler(token, instCount);
            }
         }
         else{
            currInst->imm = atoi(token);
         }

         // Error: Too large offset
         if(!isOffsetAcceptable(currInst->imm)){
            tooLargeOffsetErrorHandler(currInst->imm, instCount);
         }

         int address = registers[currInst->rs] + currInst->imm;
         setAddressValue(pMemoryTable, address, registers[currInst->rt]);
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));
      }

      if(strcmp(currInst->mnemonic, "beq") == 0){
         currInst->instType = 1;
         currInst->opCode = 11;
         // For beq instruction, the 1st register is rs and the 2nd one is rt
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         token = strtok(NULL, "\t, \n");
         if(isLable(token)){
            currInst->imm = getLableValue(pSymbolTable, symbolTableLen, token);  
            // Error: No such a label in the symbolTable
            if(currInst->imm == -1){
               undefinedLabelErrorHandler(token, instCount);
            }
         }
         else{
            currInst->imm = atoi(token);
         }

         // Error: Too large offset
         if(!isOffsetAcceptable(currInst->imm)){
            tooLargeOffsetErrorHandler(currInst->imm, instCount);
         }

         if(registers[currInst->rs] == registers[currInst->rt]){
            goToNthLine(assp, currInst->imm);
         }
         currInst->imm = currInst->imm - instCount;
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));
      }

      if(strcmp(currInst->mnemonic, "jalr") == 0){
         currInst->instType = 1;
         currInst->opCode = 12;
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         currInst->imm = 0;
         goToNthLine(assp, currInst->rs);
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));
      }

      if(strcmp(currInst->mnemonic, "j") == 0){
         currInst->instType = 2;
         currInst->opCode = 13;
         token = strtok(NULL, "\t, \n");
         if(isLable(token)){
            currInst->imm = getLableValue(pSymbolTable, symbolTableLen, token);  
         }
         else{
            currInst->imm = atoi(token);
         }
         currInst->PC = currInst->imm;
         goToNthLine(assp, currInst->imm);
         currInst->imm = currInst->imm;
         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));
      }

      if(strcmp(currInst->mnemonic, "halt") == 0){
         currInst->instType = 2;
         currInst->opCode = 14;
         currInst->imm = 0;

         formInstruction(currInst);
         writeToFile(machp, bin2Dec(currInst->instBin, 32));
      }
   }

   fclose(assp);
   fclose(machp);
   exit(0);
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
      bin = int2Binary(currInst->rs);
      strcpy(temp, binaryExtend(bin, 4, '0'));
      strcat(currInst->instBin, temp);

      bin = int2Binary(currInst->rt);
      strcpy(temp, binaryExtend(bin, 4, '0'));
      strcat(currInst->instBin, temp);

      bin = int2Binary(currInst->rd);
      strcpy(temp, binaryExtend(bin, 4, '0'));
      strcat(currInst->instBin, temp);

      strcat(currInst->instBin, "000000000000"); // unused
   }
   else if(currInst->instType == 1){
      bin = int2Binary(currInst->rs);
      strcpy(temp, binaryExtend(bin, 4, '0'));
      strcat(currInst->instBin, temp);

      bin = int2Binary(currInst->rt);
      strcpy(temp, binaryExtend(bin, 4, '0'));
      strcat(currInst->instBin, temp);

      bin = int2Binary(currInst->imm);
      if(currInst->imm < 0){
         // 2's complement extension
         bin = complement2s(bin);
         strcpy(temp, binaryExtend(bin, 16, '1'));
      }
      else{
         strcpy(temp, binaryExtend(bin, 16, '0'));
      }
      strcat(currInst->instBin, temp);
   }
   else if(currInst->instType == 2){
      strcat(currInst->instBin, "0000000000000000"); // bits 23-16 unused
      bin = int2Binary(currInst->imm);
      strcpy(temp, binaryExtend(bin, 16, '0'));
      strcat(currInst->instBin, temp);
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

int getAddressValue(struct memoryTable *pMemoryTable, int address){
   for(int i=0; i<memorySize; i++){
      if(pMemoryTable[i].address == address){
         return pMemoryTable[i].value;
      }
   }
   // No such a lable
   return -1;
}

bool setAddressValue(struct memoryTable *pMemoryTable, int address, int value){
   for(int i=0; i<memorySize; i++){
      if(pMemoryTable[i].address == address){
         pMemoryTable[i].value = value;
         return true;
      }
   }
   // No such a address
   return false;  
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

void goToNthLine(FILE *inputFile, int n){
   rewind(inputFile);
   size_t lineSize;
   int count = 0;
   char *line;
   line = (char *)malloc(72);
   while(getline(&line, &lineSize, inputFile) != -1){
      count++;
      if(count == n){
         break;
      }
   }
}

void checkDuplicateLabels(struct symbolTable *symTable, int symTableLen){
   for(int i=0; i<symTableLen-1; i++){
      for(int j=i+1; j<symTableLen; j++){
        if(strcmp(symTable[i].symbol, symTable[j].symbol) == 0){
            printf("Label \"%s\" already defined on line %d. \n", symTable[i].symbol, symTable[i].value+1);
            exit(1);
            return;
        }
      }
   }
}

bool isOffsetAcceptable(int offset){
   if(offset >= -32768 && offset <= 32767){
      return true;
   }
   return false;
}

void tooLargeOffsetErrorHandler(int offset, int lineNo){
   printf("Offset %d is too large to fit in 16 bits. Line: %d \n", offset, lineNo);
   exit(1);
}

void undefinedLabelErrorHandler(char *label, int lineNo){
   printf("Undefined label \"%s\" on line %d. \n", label, lineNo);
   exit(1);
}

long long complement2s(long long bin){
   char ones[33], twos[33];
   sprintf(ones, "%d", bin);

   int len = strlen(ones);

   // ones complement
   for(int i=0; i<len; i++){
      ones[i] = (ones[i] == '1') ? '0' : '1';
   }
   
   strcpy(twos, ones);
   int i;
   for(i=len-1; i>=0; i--){
      if(ones[i] == '1'){
         twos[i] = '0';
      }
      else{
         twos[i] = '1';
         break;
      }
   }
   if(i == -1){
      char res[33];
      res[0] = '1';
      strcat(res, twos);
      return atoi(res);
   }
   return atoi(twos);
}