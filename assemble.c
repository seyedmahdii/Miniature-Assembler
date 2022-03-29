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
   int i, j, found, noInsts;
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
   char hexTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
   char lower[5];

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

   // for(i=0; i<symbolTableLen; i++){
   //    printf("sym: %s \t val: %d\n", pSymbolTable[i].symbol, pSymbolTable[i].value);
   // }
   // printf("\n\n\n");

   while(getline(&line, &lineSize, assp) != -1){
      currInst->PC = instCount;
      instCount++;
      found = 0;
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
         printf("currInst->intInst: %d \t token: %s \n", currInst->intInst, token);

         if(memoryCounter < 2048){
            strcpy(pMemoryTable[memoryCounter].lable, lable);
            pMemoryTable[memoryCounter].value = currInst->intInst;
            // printf("%d\n", pMemoryTable[memoryCounter].value);
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
         strcpy(currInst->opCode, "1001");
         // registers[currInst->rt] = load(registers[currInst->rs] + currInst->imm);

         // printf("rt: %d /t rs: %d \t imm: %d \n", currInst->rt, currInst->rs, currInst->imm);
      }

      if(strcmp(currInst->mnemonic, "add") == 0){
         currInst->instType = 0;
         token = strtok(NULL, "\t, \n");
         currInst->rd = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rs = atoi(token);
         token = strtok(NULL, "\t, \n");
         currInst->rt = atoi(token);
         strcpy(currInst->opCode, "0000");
         registers[currInst->rd] = registers[currInst->rs] + registers[currInst->rt];
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

void formInstruction(struct instruction *, FILE *){

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