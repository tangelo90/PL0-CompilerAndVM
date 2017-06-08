#include <stdio.h>
#include <stdlib.h>
#define MAX_CHARS 2000

typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym } token_type; //enum to be used in later development

typedef struct {
    char name[12];
    int type;
                 }token; //token struct

int isreserved(char * a, char reserved[15][10]);//function to tell if word is reserved

int main(int argc, char **argv){
    char * program = calloc(MAX_CHARS, sizeof(char)); //alocate char array for program
    token* tkns = calloc(MAX_CHARS, sizeof(token)); //allocate token array for tokens

    FILE *fp;
    FILE *bp;
    fp = fopen(argv[1],"r");
    //fp = fopen("input.txt","r");
    bp = fopen("lexOutTable.txt","w");
    char c;
    int i = 0; //last index of program array
    int z = 0; //misc
    int r = 0; //misc
    int b = 0; //misc
    int u;     //misc
    int t = 0; //last index of token array
    int q; //misc
    int flag = 0; //flag

    while((c = fgetc( fp )) != EOF){
        program[i] = c;
        i++;
        if(i>1999){
            fprintf(bp,"1 program too big");
            return 0;
        }
    }//fill in program array
    fclose(fp);//close file

    /*char alpha[52]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t',
    'u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q',
    'R','S','T','U','V','W','X','Y','Z'};
    char special[14]={'+','-','*','/','(',')','=',',','.','<','>',';',':'};
    char digits[10]={'0','1','2','3','4','5','6','7','8','9'};*/

    char reserved[15][10] = {"begin","end","if","then","while","do","call","const","var","procedure","write","read","else","odd","nul"}; //array for reserved words


    while(1){
        if(z==i){
            b = 1;
        }//check if end has been reached

        if(flag == 1){
            return 0;
        }//check if flag has been triggered

        u=0; //set tracking var to 0

        if(program[z]=='+'){
        tkns[t].name[u] = '+';
        tkns[t].type = 4;
        t++;
        z++;
        } //case for +

        else if(program[z]=='-'){
            tkns[t].name[u] = '-';
            tkns[t].type = 5;
            t++;
            z++;
        } //case for -

        else if(program[z]=='*'){
            tkns[t].name[u] = '*';
            tkns[t].type = 6;
            t++;
            z++;
        }//case for *

        else if(program[z]=='/'){
            z++;
            if(program[z]=='*'){
                while(1){
                    z++;
                    if(program[z]=='*'){
                        z++;
                        if(program[z]=='/')
                            z++;
                            break;
                    }
                }
            }//check for comment
            else{
                tkns[t].name[u] = '/';
                tkns[t].type = 7;
                t++;
            }
        }//case for /

        else if(program[z]=='('){
            tkns[t].name[u] = '(';
            tkns[t].type = 15;
            t++;
            z++;
        }//case for (

        else if(program[z]==')'){
            tkns[t].name[u] = ')';
            tkns[t].type = 16;
            t++;
            z++;
        }//case for )

        else if(program[z]=='='){
            tkns[t].name[u] = '=';
            tkns[t].type = 9;
            t++;
            z++;
        }//case for =

        else if(program[z]=='<'){
            z++;
            if(program[z]=='>'){
                tkns[t].name[u] = '<';
                tkns[t].name[u+1] = '>';
                tkns[t].type = 10;
                t++;
                z++;
            }//case for not equal
            else if(program[z]=='='){
                tkns[t].name[u] = '<';
                tkns[t].name[u+1] = '=';
                tkns[t].type = 12;
                t++;
                z++;
            }//case for less than or equal
            else{
                tkns[t].name[u] = '<';
                tkns[t].type = 11;
                t++;
            }//case for less than
        }//case for <

        else if(program[z]=='>'){
            z++;
            if(program[z]=='='){
                tkns[t].name[u] = '>';
                tkns[t].name[u+1] = '=';
                tkns[t].type = 14;
                t++;
                z++;
            }//case for greater than or equal
            else{
                tkns[t].name[u] = '>';
                tkns[t].type = 13;
                t++;
            }//case for greater than
        }

        else if(program[z]==','){
            tkns[t].name[u] = ',';
            tkns[t].type = 17;
            t++;
            z++;
        }//case for comma

        else if(program[z]=='.'){
            tkns[t].name[u] = '.';
            tkns[t].type = 19;
            t++;
            z++;
        }//case for period

        else if(program[z]==';'){
            tkns[t].name[u] = ';';
            tkns[t].type = 18;
            t++;
            z++;
        }//case for semicolon

        else if(program[z]==':'){
            z++;
            if(program[z]=='='){
                tkns[t].name[u] = ':';
                tkns[t].name[u+1] = '=';
                tkns[t].type = 20;
                t++;
                z++;
            }
            else{
                fprintf(bp,"1 colon without equal");
                flag = 1;
            }
        }//case for :

        else if(program[z]=='\n'){
            z++;
        }
        else if(program[z]=='\t'){
            z++;
        }
        else if(program[z]=='\r'){
            z++;
        }
        else if(program[z]==' '){
            z++;
        }//empty space cases

        else if((program[z]>='a' && program[z]<='z')||(program[z]>='A' && program[z]<='Z')){
            tkns[t].name[u] = program[z];
            q=u;//use work var to store 0
            z++;
            u++;
            while((program[z]>='a' && program[z]<='z')||(program[z]>='A' && program[z]<='Z')||(program[z]>='0' && program[z]<='9')){
                if(u>11){
                    flag = 1;
                    fprintf(bp, "1 Error word too long");
                    break;
                }//case of word too long
                tkns[t].name[u] = program[z];
                u++;
                z++;
            }//go through word until too long or non letter/number
            tkns[t].type = isreserved(&tkns[t].name[q],reserved);//determine type of word
            t++;
        }//case for letter

        else if(program[z]>='0' && program[z]<='9'){
            tkns[t].name[u] = program[z];
            u++;
            z++;
            while(program[z]>='0' && program[z]<='9'){
                if(u>4){
                    flag = 1;
                    fprintf(bp,"1 Error num too long");
                    break;
                }//for when number more than 5 digits
                tkns[t].name[u] = program[z];
                z++;
                u++;
            }//case for number
            if((program[z]>='a' && program[z]<='z')||(program[z]>='A' && program[z]<='Z')){
                fprintf(bp,"1 Error invalid word");
                flag = 1;
            }//case for when there is a letter after a number
            tkns[t].type = 3;
            t++;
        }

        else if(program[z]!='\0'){
        fprintf(bp,"1 Error invalid char %c", program[z]);
        flag = 1;
        }//case for when not at end of file and the above expressions do not recognize char

        if(b==1){
            break;
        }//break loop if at end of program chars
    }


    /*fprintf(fp,"Source Program:\n");
    for(z=0; z<i; z++){
        fprintf(fp,"%c",program[z]);

    }
    fprintf(fp,"\nLexeme Table:\nlexeme		  token type\n");
    for(z=0; z<t; z++){
    fprintf(fp,"%s		        %d\n",tkns[z].name, tkns[z].type);

    }
    fprintf(fp,"\nLexeme List:\n");
    for(z=0; z<t; z++){
        if(tkns[z].type==2 || tkns[z].type==3){
            fprintf(fp,"%d %s  ",tkns[z].type, tkns[z].name);
        }
        else{
            fprintf(fp,"%d  ",tkns[z].type);
        }
    }//all print statements for outputs
    */
    if(t>299){
            fprintf(bp,"1 program too big");
            return 0;
        }
    fprintf(bp, "0 ");
    for(z=0; z<t; z++){
    fprintf(bp,"%s %d",tkns[z].name, tkns[z].type);
    if(z<t-1){
        fprintf(bp," ");
    }
    }
    fclose(bp);//close file
    return 0;
}

int isreserved(char * a, char reserved[15][10]){
    //const, var, procedure, call, begin, end, if, then, else, while, do, read, write, nul, odd.
    int i,z,e;
    e=0;//equal var

    for(i=0; i<13; i++){
        for(z=0; z<10; z++){
            if(a[z] != reserved[i][z]){
                e=0;
                break;
            }
            e=1;
        }
        if(e==1){
            return i+21;
        }
    }//check for first 13 reserved words

    for(z=0;z<10;z++){
        if(a[z] != reserved[i][z]){
                e=0;
                break;
            }
            e=1;
        }
        if(e==1){
            return 8;
        }//check for odd

    i++;//increment i

    for(z=0;z<10;z++){
        if(a[z] != reserved[i][z]){
                e=0;
                break;
            }
            e=1;
        }//check for nul
        if(e==1){
            return 1;
        }

    return 2;
}
