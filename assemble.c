#include "assemble.h"

void main(int argc,char **argv){
   FILE *assp, *machp, *fopen();
   struct symbolTable *pSymbolTable;
   int symbolTableLen;
   int i, j, found, noInsts;
   struct instruction *currInst;
   size_t lineSize;
   char *line;
   char *token;
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

void formatInstruction(struct instruction *, FILE *){

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