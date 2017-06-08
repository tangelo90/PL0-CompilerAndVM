#include <stdio.h>
#include <stdlib.h>


//all of these could be put into a header file or elsewhere to make everything cleaner, up to you guys
typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym } token_type;

typedef enum{lit = 1, rtn, lod, sto, cal, inc, jmp, jpc, printreg, readreg, halt, neg, add, sub, mul, divi, odd, mod, eql, neq, lss,
leq, gtr, geq} op_type;

//token struct
typedef struct  {
    char name[12];
    int type;
                 }token;

//symbol struct, did not do typedef because of weird interactions with symbol node
struct symbol{
    int kind;   // const = 1, var = 2, proc = 3
    char name[12]; // name up to 11 chars
    int val;   // number (ASCII value)
    int level;   // L level
    int addr;   // M address
    struct symbol* next;

};

typedef struct{
    int op;
    int r;
    int l;
    int m;

} instruction;

//globals
struct symbol* table[50]; //symbol table
instruction code[500];
token tknlist[300]; //list of tokens, can be fed from the lexer directly or through txt file
token tkn; //global token
int flag = 0; //flag may be not needed
int level = 0; //global to keep track of the lexical level the code is on
int tknindex = 0; //current index in the token list
int cdx = 0;
int address = 0; //to assign var and constant stack addresses
int reg = -1;

//get token
void get_token();

//symbol functions
void enter(int type, char* id, int param);
struct symbol* read(char* id);
void delsyms();
int getSymbols();

//error function
void error(int num);

//parser functions
void program();
void block();
void var_decl();
void constant_decl();
void proc_decl();
void statement();
void expression();
void condition();
void term();
void factor();
void skipTo(int function);
int skipSearch( int length, int *function);

//helper function
int str_to_num(char *s);

//output
void emit(int op, int l, int m, int idx);
void writeOutput(int i, int syntax);


int main(){
	int i = 0;
	char c;
	// read in symbols from lexer
	i = getSymbols();
    // open and dclose file to make sure it's empty
    FILE *fp = fopen("errors.txt", "w");
    FILE *zp;
    fprintf(fp, " ");
    fclose(fp);
	// analyze symbols
    program();
    // write interpreter assembly to output
    if(flag == 0) writeOutput(cdx, 1);
    else{
        fp = fopen("parserOut.txt", "w");
        zp = fopen("errors.txt", "r");
        fprintf(fp,"1 \n");
        while(!feof(fp)){
            c = getc(zp);
            fprintf(fp,"%c",c);
        }
        
    }
    return 0;
}

int getSymbols() {
	FILE *fp = NULL;
	fp = fopen("lexOutTable.txt", "r");

	// check if file opened correctly
	if(fp == NULL) {
		printf("%s\n", "Error finding lexer output file");
		return 0;
	}

	// token form is char name int type
	int i = 0;
	int f;
	fscanf(fp,"%d",&f);
	while (!feof(fp)) {
		fscanf(fp, "%s %d", tknlist[i].name, &tknlist[i].type);
		i++;
	}
	fclose(fp);
	return i; // returns number of tokens added
	// to iterate through list simply use :
	// int j = 0;
	// 	for(j = 0; j < i; j++){
	// 		printf("Token is: %s %d\n", tknlist[j].name, tknlist[j].type);
	// 	}
}


void enter(int type, char* id, int param){
    int i;
    int index;
    struct symbol *temp;
    struct symbol * s;
    if(type==1){
       s = calloc(1, sizeof(struct symbol));
       for(i=0;i<12;i++){
       s -> name[i] = id[i];
       if(id[i]=='\0') break;
       }
       s->kind = type;
       s->addr = param;
       s->level = level;
       s->next = NULL;
    } //case for variable
    else if(type==2){
       s = calloc(1, sizeof(struct symbol));
       for(i=0;i<12;i++){
       s -> name[i] = id[i];
       if(id[i]=='\0') break;
       }
       s->kind = type;
       s->val = param; //store value
       s->level = level;
       s->next = NULL;
    }//case for constant
    else if(type==3){
         s = calloc(1, sizeof(struct symbol));
       for(i=0;i<12;i++){
       s -> name[i] = id[i];
       if(id[i]=='\0') break;
       }
       s->kind = type;
       s->addr = param;
       s->level = level;
       s->next = NULL;
    }

    index = (id[0]+id[1])%50;//assign index as sum of first 2 chars mod 50

    if(table[index]==0){
        table[index] = s;
    }//insert into table if slot is empty

    else{
        temp = table[index];
        while(temp->next != NULL){
            temp = temp -> next;
        }
        temp -> next = s;
    }//go through and insert at end of slot

}

struct symbol* read(char * id){
    int index;
    int i;
    int found = 0;
    int lev = level;
    struct symbol *temp;
    index = (id[0]+id[1])%50;
    if (table[index]==0) {
        error(11);
    } 
    while(lev >= 0){
    temp = table[index];
    while(1){
        for(i=0;i<12;i++){

            if(id[i]==temp->name[i]){
                found = 1;
            }
            else found = 0;

            if(id[i]=='\0') break;
        }//compare names

        if(lev != temp->level)found = 0; //compare levels

        if(found == 1) break;//if found break

        else{
            temp = temp->next;
        }

        if (temp == NULL){
            if(lev==0)error(11);
            break;
        }//if at end of list error

    }
    if(found == 1) break;
    lev--;
    }
    return temp; //return pointer to found symbol

}

void get_token(){
    int i;
    tkn.type = tknlist[tknindex].type;
    for(i=0;i<12;i++){
       tkn.name[i] = tknlist[tknindex].name[i];
       if( tkn.name[i]=='\0') break;
       }
    tknindex++;
}//get token from token list, insert into global token


void writeOutput(int i, int syntax) {


    FILE *fp = fopen("parserOut.txt", "w");
    fprintf(fp,"0 \n");
    if(fp == NULL)
    	printf("Error while opening output file for parser ...\n");

    if(syntax == 1)
        fprintf(fp, "%s\n", "The program is syntactically correct.");
    else
        fprintf(fp, "%s\n", "The program contained some errors and was compiled with them.");

    int j = 0;
    for(j = 0; j < i; j++)
    	fprintf(fp, "%d %d %d %d\n", code[j].op, code[j].r, code[j].l, code[j].m);

    fclose(fp);
}


void program(){
    get_token();
    block();
    if (tkn.type != periodsym) error(9);
        emit(halt,0,3,cdx);
}


void proc_decl(){
    while(tkn.type == procsym){
        get_token();
        if (tkn.type != identsym) {
            error(6);
            skipTo(7);
        } 
        enter(3,tkn.name,cdx);
        get_token();
        if (tkn.type != semicolonsym) {
            error(17);
            skipTo(7);
            return;
        } 
        get_token();
        level++;
        block();
        if(tkn.type != semicolonsym) {
            error(17);
            skipTo(7);
            return;
        } 
        delsyms();
        level--;
        get_token();
        emit(rtn,0,0,cdx);
    }
}

void constant_decl(){
    token temp;
    int num;
    do{
        get_token();
        if (tkn.type != identsym) {
            error(4);
            skipTo(9);
            return;
        }
        temp = tkn;
        get_token();
        if (tkn.type != eqsym) {
            error(13);
            skipTo(9);
            return;
        }
        get_token();
        if (tkn.type != numbersym) {
            error(2);
            skipTo(9);
            return;
        }
        num=str_to_num(tkn.name);
        enter(2,temp.name,num);
        get_token();
    } while(tkn.type == commasym);
    if (tkn.type != semicolonsym) {
        error(17);
        skipTo(9);
        return;
    }
    get_token();
}//need to add gen code to put value on stack

void var_decl(){
    do{
        get_token();
        if (tkn.type != identsym) {
            error(4);
            skipTo(8);
            return;
        }
        address++;
        enter(1,tkn.name,address);
        get_token();

    } while(tkn.type == commasym);

    if (tkn.type != semicolonsym)  {
            error(17);
            skipTo(8);
            return;
    }
    get_token();

}

void block(){
    int temp, temp2;
    temp = address;
    temp2 = cdx;
    cdx++;
    address = address+4;
    if (tkn.type == constsym) constant_decl();
    if (tkn.type == varsym) var_decl();
    temp = address-temp;
    address = 0;
    if(tkn.type == procsym) proc_decl();
    emit(jmp,0,cdx,temp2);
    emit(inc,0,temp,cdx);
    statement();
}

void statement(){
    struct symbol * s;
    int temp;
    int temp2;
    if (tkn.type == identsym){
        s=read(tkn.name);
        if(s->kind != 1) {
            error(12);
            skipTo(2);
            return;
        }
        get_token();
        if (tkn.type != becomessym) {
            error(13);
            skipTo(2); 
            return; 
        } 
        get_token();
        expression();
        emit(sto,level-s->level,s->addr,cdx);
        reg--;
    }
    else if(tkn.type==callsym){
        get_token();
        if (tkn.type!=identsym) {
            error(14);
            skipTo(2);
            return;
        } 
        s = read(tkn.name);
        if (s->kind != 3) {
            error(15);
            skipTo(2); 
            return;
        }
        emit(cal,level - s->level,s->addr,cdx);
        get_token();
    }
    else if(tkn.type==beginsym){
        get_token();
        statement();
        while(tkn.type==semicolonsym){
            get_token();
            statement();
        }
        if(tkn.type!=endsym) {
            error(9);
            skipTo(2);
            return;
        }
        get_token();
    }
    else if(tkn.type == ifsym){
        get_token();
        condition();
        reg--;
        if(tkn.type!=thensym) {
            error(16); 
            skipTo(2);
            return;
        } 
        temp = cdx;
        cdx++;
        get_token();
        statement();
        temp2 = cdx;
        emit(jmp,0,0,cdx);
        emit(jpc,0,cdx,temp);
        if(tkn.type==elsesym){
            statement();
        }
        code[temp2].m = cdx;
    }
    else if(tkn.type == whilesym){
        temp = cdx;
        get_token();
        condition();
        reg--;
        temp2 = cdx;
        cdx++;
        if(tkn.type!=dosym) {
            error(18);
            skipTo(2);
            return;
        } 
        get_token();
        statement();
        emit(jmp,0,temp,cdx);
        emit(jpc,0,cdx,temp2);
    }
    else if(tkn.type == readsym){
        get_token();
        if(tkn.type!=identsym) {
            error(26);
            skipTo(2);
            return;
        }
        emit(readreg,0,2,cdx);
        s = read(tkn.name);
        if(s == NULL){
            skipTo(2);
            return;
        }
        if(s->kind!=1) {
            error(26);
            skipTo(2);
            return;
        }
        emit(sto,level-s->level,s->addr,cdx);
        get_token();
    }
    else if(tkn.type == writesym){
        get_token();
        if(tkn.type!=identsym) {
            error(27);
            skipTo(2);  
        } 
        s = read(tkn.name);
        if(s == NULL){
            skipTo(2);
            return;
        }
        if(s->kind == 3){
            error(28);
            skipTo(2);
            return;
        }
        reg++;
        if (s->kind == 1) emit(lod,level-s->level,s->addr,cdx);
        else emit(lit,0,s->val,cdx);
        emit(printreg,0,1,cdx);
        reg--;
        get_token();
    }

}

void condition(){
    int optype;
    if (tkn.type==oddsym){
        get_token();
        expression();
        emit(odd,reg,0,cdx);
    }
    else{
        expression();
        optype=tkn.type;
        if(eqsym>tkn.type || tkn.type>geqsym) {
            error(20);
            skipTo(3);
        } 
        get_token();
        expression();
        if(optype==eqsym)emit(eql,reg-1,reg,cdx);
        else if(optype==neqsym) emit(neq,reg-1,reg,cdx);
        else if(optype==lessym) emit(lss,reg-1,reg,cdx);
        else if(optype==leqsym) emit(leq,reg-1,reg,cdx);
        else if(optype==gtrsym) emit(gtr,reg-1,reg,cdx);
        else if(optype==geqsym) emit(geq,reg-1,reg,cdx);
        reg--;
    }
//eqsym, neqsym, lessym, leqsym, gtrsym, geqsym
//eql, neq, lss,leq, gtr, geq
}

void expression(){
    int adop;
    if(tkn.type == plussym || tkn.type == minussym){
        adop = tkn.type;
        get_token();
        term();
        if(adop == minussym) emit(neg,reg,0,cdx);

    }
    else term();
    while(tkn.type == plussym || tkn.type == minussym){
        adop = tkn.type;
        get_token();
        term();
        if(adop == plussym) emit(add,reg-1,reg,cdx);

        else emit(sub,reg-1,reg,cdx);

        reg--;
    }

}

void term(){
   int mulop;
   factor();
   while (tkn.type == multsym || tkn.type == slashsym){
        mulop = tkn.type;
        get_token();
        factor();
        if(tkn.type == multsym) emit(mul,reg-1,reg,cdx);

        else emit(divi,reg-1,reg,cdx);

        reg--;
   }

}

void factor(){
    struct symbol * s;
    if(tkn.type==identsym){
        s=read(tkn.name);
        if(s->kind==3) {
            error(21);
            skipTo(6);
        }
        reg++;
        if(s->kind==1)emit(lod,level-s->level,s->addr,cdx);
        else emit(lit,0,s->val,cdx);
        get_token();
        }

    else if(tkn.type == numbersym) {
        reg++;
        emit(lit,0,str_to_num(tkn.name),cdx);
        get_token();}

    else if(tkn.type == lparentsym){
        get_token();
        expression();
        if(tkn.type != rparentsym) {
            error(22);
            skipTo(6);
        }
        get_token();
    }

    else {
        error(23);
        skipTo(6);
    }

}



void error(int errCode) {
    flag = 1;
    FILE *fp = fopen("errors.txt", "a+");
	switch(errCode) {
		case 1:
			fprintf(fp," Error 1 Use = instead of :=.\n");
			break;
		case 2:
			fprintf(fp," Error 2 = must be followed by a number.\n");
			break;
		case 3:
			fprintf(fp," Error 3 Identifier must be followed by =.\n");
			break;
		case 4:
			fprintf(fp," Error 4 const, var, procedure must be followed by identifier.\n");
			break;
		case 5:
			fprintf(fp," Error 5 comma missing.\n");
			break;
		case 6:
			fprintf(fp," Error 6 Incorrect symbol after procedure declaration.\n");
			break;
		case 7:
			fprintf(fp," Error 7 Statement expected.\n");
			break;
		case 8:
			fprintf(fp," Error 8 Incorrect symbol after statement part in block.\n");
			break;
		case 9:
			fprintf(fp," Error 9 Period expected.\n");
			break;
		case 10:
			fprintf(fp," Error 10 Semicolon between statements missing.\n");
			break;
		case 11:
			fprintf(fp," Error 11 Undeclared identifier.\n");
			break;
		case 12:
			fprintf(fp," Error 12 Assignment to constant or procedure is not allowed.\n");
			break;
		case 13:
			fprintf(fp," Error 13 Assignment operator expected.\n");
			break;
		case 14:
			fprintf(fp," Error 14 call must be followed by an identifier.\n");
			break;
		case 15:
			fprintf(fp," Error 15 Call of a constant or variable is meaningless.\n");
			break;
		case 16:
			fprintf(fp," Error 16 then expected.\n");
			break;
		case 17:
			fprintf(fp," Error 17 Semicolon expected.\n");
			break;
		case 18:
			fprintf(fp," Error 18 do expected.\n");
			break;
		case 19:
			fprintf(fp," Error 19 Incorrect symbol following statement.\n");
			break;
		case 20:
			fprintf(fp," Error 20 Relational operator expected.\n");
			break;
		case 21:
			fprintf(fp," Error 21 Expression must not contain a procedure identifier.\n");
			break;
		case 22:
			fprintf(fp," Error 22 Right parenthesis missing.\n");
			break;
		case 23:
			fprintf(fp," Error 23 The preceding factor cannot begin with this symbol.\n");
			break;
		case 24:
			fprintf(fp," Error 24 An expression cannot begin with this symbol.\n");
			break;
		case 25:
			fprintf(fp," Error 25 This number is too large.\n");
			break;
		case 26:
			fprintf(fp," Error 26 Expected Var after read.\n");
			break;
		case 27:
			fprintf(fp," Error 27 Expected Ident.\n");
			break;
		case 28:
			fprintf(fp," Error 28 Cannot write procedure.\n");
			break;
		}
    fclose(fp);
}

void emit(int op, int l, int m, int idx){
    if(op==sto){
        code[idx].op=op;
        code[idx].r=0;
        code[idx].l=l;
        code[idx].m=m;
    }
    else if(op==lit || op == lod){
        code[idx].op=op;
        code[idx].r=reg;
        code[idx].l=l;
        code[idx].m=m;
    }
    else{
        code[idx].op=op;
        code[idx].r=l;
        code[idx].l=l;
        code[idx].m=m;
    }

    if(idx==cdx)cdx++;
}

int str_to_num(char* s){
    int i;
    int n = 0;
    for(i=0;i<12;i++){
        if(s[i]=='\0') break;
        n = 10*n + s[i]%48;

    }
    return n;

} //helper string to number function

void delsyms(){
    int i;
    int f=0;
    struct symbol * temp1;
    struct symbol * temp2;
    struct symbol * prior;
    for(i=0; i<50; i++){
        if(table[i] != 0){
            temp1 = table[i];
            prior = temp1;
            do{
            if(f==1) free(temp2);
            temp2=temp1;
            f=0;
            if (temp1 -> level == level ){
                if(temp1 == table[i]) table[i] = temp1 -> next;
                else {
                    prior -> next = temp1 -> next;

                }
                f=1;
            }
            
                prior = temp1;
                temp1 = temp1 ->next;
            } while(temp2 -> next != NULL);
        }

    }

}

int skipSearch(int length, int *function) {
    int i;
    int flag = 1;
    while(flag) {
        get_token();
        for(i = 0; i < length; i++) {
            if(tkn.type == function[i])
                return 1;
        }       
    }
    return 0;
}

// skips to the 
void skipTo(int function) {
    FILE* fp;
    FILE* zp;
    char c;
    int result = 0;
    int blockFS[2] = {periodsym, semicolonsym};
    int StatementFS[3] = {periodsym, semicolonsym, endsym};
    int ConditionFS[2] = {thensym, dosym};
    int ExpressionFS[6] = {periodsym, semicolonsym, rparentsym, endsym, thensym, dosym};
    int TermFS[8] = {periodsym, semicolonsym, rparentsym, endsym, thensym, dosym, plussym, minussym};
    int FactorFS[10] = {periodsym, semicolonsym, rparentsym, endsym, thensym, dosym, plussym, minussym, slashsym, multsym};
    int procFS[9] = {identsym, callsym, beginsym, ifsym, whilesym, readsym, writesym, procsym,periodsym};
    int vardecFS[9] = {identsym, callsym, beginsym, ifsym, whilesym, readsym, writesym, procsym,periodsym};
    int constDecFS[10] = {varsym,identsym, callsym, beginsym, ifsym, whilesym, readsym, writesym, procsym,periodsym};


    switch(function) {
        case 1:
            result = skipSearch( 2, blockFS);
            break;
        case 2:
            result = skipSearch( 3, StatementFS);
            break;
        case 3:
            result = skipSearch( 2, ConditionFS);
            break;
        case 4:
            result = skipSearch( 6, ExpressionFS);
            break;
        case 5:
            result = skipSearch( 8, TermFS);
            break;
        case 6:
            result = skipSearch( 10, FactorFS);
            break;
        case 7:
            result = skipSearch( 9, procFS);
            break;
        case 8:
            result = skipSearch( 9, vardecFS);
            break;
        case 9:
            result = skipSearch( 10, constDecFS);
            break;
        default:
            result = 0;
            break;

    }
    if(!result) { //reached end of file
        fp = fopen("parserOut.txt", "w");
        zp = fopen("errors.txt", "r");
        fprintf(fp,"1 \n");
        while(!feof(zp)){
            c = getc(zp);
            fprintf(fp,"%c",c);
        }
        exit(0);
    } 
}
