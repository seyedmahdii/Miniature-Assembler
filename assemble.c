// #include<stdio.h>
// #include<stdlib.h>
#include "assemble.h"

void main(int argc,char **argv){
   FILE *assp, *machp, *fopen();
   struct symbolTable *pSymbolTable;
   int symbolTableLen;

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
