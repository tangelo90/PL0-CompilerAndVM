#include <stdio.h>
#include <stdlib.h>
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct instruction{
    int op;	// opcode
    int r; 	// reg
    int  l;	// L
    int  m;	// M
}instruction;

int base(int l,int base, int* stack);
void fetch(instruction* code, int* pc, instruction* ir);
void execute(instruction ir, int* stack, int* pc, int* sp, int* bp, int* r, int* flag);

int main(){
    int i = 0;
    int z;
    int u;
    char filler[15];
    FILE *fp;
    FILE *np;
    fp = fopen("parserOut.txt","r");
    np = fopen("vmExecuteTrace.txt","w");
    int* stack = calloc(MAX_STACK_HEIGHT, sizeof(int));
    int* regs = calloc(16, sizeof(int));
    instruction* code = calloc(MAX_CODE_LENGTH, sizeof(instruction));
    
    fscanf(fp, "%d", &u);
    for(z=0; z<5; z++){
        fscanf(fp, "%s", filler);}
        
    while(fscanf(fp,"%d %d %d %d",&code[i].op,&code[i].r,&code[i].l,&code[i].m) != EOF ){
        i++;
        if(i>=(MAX_CODE_LENGTH)){
           fprintf(np,"code too long");
           return 0;
        }
    }//fill in code array

    fclose(fp); //close file

    //fp = fopen("vmout.txt","w");

    char ops[24][4] = {"lit","rtn","lod","sto","cal","inc","jmp","jpc",
    "sio","sio","sio","neg","add","sub","mul","div","odd","mod","eql",
    "neq","lss","leq","gtr","geq"}; //define string array for op names

    //fprintf(fp,"Line OP   R L M\n");
    //for(z=0;z<i;z++){
        //fprintf(fp," %d   %s  %d %d %d\n",z,ops[code[z].op-1],code[z].r,code[z].l,code[z].m); //output program to file
    //}
    //fprintf(fp,"\n");

    int frames[500];//create frame array to store frame positions
    int flag = 0; //halting condition
    int pc = 0; //program counter
    int sp = 0; //stack pointer
    int bp = 1; //base pointer
    int c = 0; //misc
    int tmp = 0; //work variable
    instruction ir; //instruction struct
    ir.op = 0;
    ir.r = 0;
    ir.l = 0;
    ir.m = 0; //zero out intruction values

    fprintf(np,"Inital Values			 pc    bp   sp\n"); //format output file

    while(1){
    z=0;
    c=pc;
    fetch(code,&pc,&ir); //fetch code
    execute(ir,stack,&pc,&sp,&bp,regs,&flag); //execute code
    if(flag == 1) break; //break if flag is 1

    fprintf(np,"%d	%s 	 %d	 %d	 %d	 %d	 %d	 %d  ",c,ops[ir.op-1],ir.r,ir.l,ir.m,pc,bp,sp); //output initial values

    tmp = bp; //assign bp to tmp

    while(tmp>1){
        frames[z] = tmp;
        tmp = stack[tmp+2];
        z++;
    } //fill in frames, change in code

    for(i = 1; i < sp; i++){
        if(i==frames[z-1]){
        fprintf(np," | ");
        z--;
        }
        fprintf(np,"%d ",stack[i]);
    } //print out stack

    fprintf(np,"%d ",stack[sp]);

    if(sp >= MAX_STACK_HEIGHT) break; //break if stack runs out of space
    if(flag==2) break; //change in code
    fprintf(np,"\n");
    }

    fclose(np); //close file
    return 0;
    }


int base(int l, int base, int* stack) // l stand for L in the instruction format
{
  int b1; //find base L levels down
  b1 = base;
  while (l > 0)
  {
    b1 = stack[b1+1];
    l--;
  }
  return b1;
}

void fetch(instruction* code, int* pc, instruction* ir){
    ir->op = code[*pc].op;
    ir->m = code[*pc].m;
    ir->l = code[*pc].l;
    ir->r = code[*pc].r;
    *pc = *pc+1;
} //function to fill in struct and increment pc

void execute(instruction ir, int* stack, int* pc, int* sp, int* bp, int* r, int* flag){
    switch(ir.op){
        case  1:
            r[ir.r] = ir.m;
            break; //LIT
        case  2:
             if(*bp == 1){
                *flag = 2;
            }//change in code
            *sp = *bp-1;
            *bp = stack[*sp+3];
            *pc = stack[*sp+4];
            break;//RTN
        case  3:
            r[ir.r] = stack[base(ir.l,*bp,stack)+ir.m];
            if(ir.l>MAX_LEXI_LEVELS){
                *flag = 1;
            }//set flag to one if lexi levels too much
            break; //LOD
        case  4:
            stack[base(ir.l,*bp,stack)+ir.m] = r[ir.r];
            if(ir.l>MAX_LEXI_LEVELS){
                *flag = 1;
            }//set flag to one if lexi levels too much
            break; //STO
        case  5:
            stack[*sp+1] = 0;
            stack[*sp+2] = base(ir.l,*bp,stack);
            stack[*sp+3] = *bp;
            stack[*sp+4] = *pc;
            *bp = *sp+1;
            *pc = ir.m;
            if(ir.l>MAX_LEXI_LEVELS){
                *flag = 1;
            }//set flag to one if lexi levels too much
            break; //CAL
        case  6:
            *sp = *sp + ir.m;
            break; //INC
        case  7:
            *pc = ir.m;
            break; //JMP
        case  8:
            if(r[ir.r]==0){
                *pc =  ir.m;
            }
            break; //JPC
        case  9:
            printf("%d\n",r[ir.r]);
            break; //SIO WRITE
        case  10:
            scanf("%d",&r[ir.r]);
            break; //SIO READ
        case  11:
            *flag = 1;
            break; //SIO END
        case  12:
            r[ir.r] = -r[ir.m];
            break; //NEG
        case  13:
            r[ir.r] = r[ir.l] + r[ir.m];
            break; //ADD
        case  14:
            r[ir.r]=r[ir.l] - r[ir.m];
            break; //SUB
        case  15:
            r[ir.r] = r[ir.l] * r[ir.m];
            break; //MUL
        case  16:
            r[ir.r] = r[ir.l] / r[ir.m];
            break; //DIV
        case  17:
            r[ir.r] = r[ir.r] % 2;
            break; //ODD
        case  18:
            r[ir.r] = r[ir.l] % r[ir.m];
            break; //MOD
        case  19:
            r[ir.r] = r[ir.l] == r[ir.m];
            break; //EQL
        case  20:
            r[ir.r] = r[ir.l] != r[ir.m];
            break; //NEQ
        case  21:
            r[ir.r] = r[ir.l] < r[ir.m];
            break; //LSS
        case  22:
            r[ir.r] = r[ir.l] <= r[ir.m];
            break; //LEG
        case  23:
            r[ir.r] = r[ir.l] > r[ir.m];
            break; //GTR
        case  24:
            r[ir.r] = r[ir.l] >= r[ir.m];
            break; //GEQ
        default:
            *flag = 1; //no valid ops flag is set to 1


    }



}