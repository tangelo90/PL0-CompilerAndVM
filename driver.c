#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define lexemeList "-l"
#define generatedAssembly "-a"
#define vmExecutionTrace "-v"

//token struct
typedef struct  {
    char name[12];
    int type;
                 }token;
token tknlist[300];
const char* tokenstr[] = {"nulsym","identsym","numbersym","plussym","minussym","multsym","slashsym",
        "oddsym","eqsym","neqsym","lessym","leqsym","gtrsym","geqsym","lparentsym","rparentsym","commasym",
        "semicolonsym","periodsym","becomessym","beginsym","endsym","ifsym","thensym","whilesym","dosym","callsym",
            "constsym","varsym","procsym","writesym","readsym","elsesym"};

// Handles errors for command line input
// Types : 1 = fatal, 2 = invalid
void DriverError(int type, char *message) {
	switch(type) {
		case 1:
			printf("FATAL ERROR! Program Aborted! ... \n Error While Attempting to run given input ... \n\n%s \n(Note: Correct form is ./exe [-v -l -a]) \n\n", message);
			return;
		case 2:
			printf("RUNTIME ERROR! ... \n\n%s \n\n", message);
			return;
		case 3:
			printf("Alert! Message ... \n%s \n\n", message);
			return;
		case 4:
			printf("FATAL RUNTIME ERROR! Program Aborted! ... \n\n%s \n\n", message);
			return;
		default:
			printf("\n [Message: ' %s '] \n", message);
			break;
	}

}
// print file input for no directive option
void printInput(char *inputFile) {
	printf("Input: \n %s \n", inputFile);

}
//format tokens
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
// print file output for no directive option, output text will come from another file
void printOutput(char *outputFile) {
	int i = 0, sl = 0;
	sl = strlen(outputFile); // just a length check, can remove
	if(sl < 1)
		printf("%s\n", "No Output Found");
	else
		printf("Output: \n %s \n", outputFile);

}


// decodes the valid directive and executes the neccessary code
void ExecuteDirectives(int argcount, char** defaultDirectives, char **argv) {

	int i = 0, j = 0, comp = 0, track = 0, z = 0, q = 0;

	char string[100]; // for printing from output files

	// decode and execute driectives
	for(i=1; i < argcount; i ++) {
		if(track > argcount)
			DriverError(1, "Directive processing error.");
		for(j=0; j < 3; j++) {
			comp = strcmp(argv[i], defaultDirectives[j]);
			if(!comp)
			{
				//[0] = lexemeList;
				//[1] = generatedAssembly;
				//[2] = vmExecutionTrace;
				switch(j) {
					case 0:
						printf("\n Executing ... [%s] Lexeme/Tokens: \n\n", lexemeList);
						FILE *lex = fopen("lexOutTable.txt", "r");
						if(lex == NULL) {
							DriverError(1, "Lexer did not produce valid output, or error reading lexOutTable.txt");
							break;
						}

						z=getSymbols();
						for(q=0; q<z; q++){
						    if(tknlist[q].type==2 || tknlist[q].type==3)
                            printf("%d %s ",tknlist[q].type,tknlist[q].name);
                            else printf("%d ",tknlist[q].type);
						}
						printf("\n");
						for(q=0; q<z; q++){
						    if(tknlist[q].type==2 || tknlist[q].type==3)
                            printf("%s %s ",tokenstr[tknlist[q].type-1],tknlist[q].name);
                            else printf("%s ",tokenstr[tknlist[q].type-1]);
						}
						printf("\n");
						break;
					case 1:
						printf("\n Executing ... [%s] Generated Assembly: \n\n", generatedAssembly); // --*dev note for later* --
						FILE *ga = fopen("parserOut.txt", "r");
						if(ga == NULL) {
							DriverError(1, "Parser did not produce valid output, or error reading parserOut.txt");
							break;
						}
						fscanf(ga,"%d",&q);
						while(fgets(string, 100, ga)) {
							printf("%s", string);
						}
						break;
					case 2:
						printf("\n Executing ... [%s] Execution Trace: \n\n", vmExecutionTrace); // --*dev note for later* --
						 //compile VM code, .c name may change
						FILE *vm = fopen("vmExecuteTrace.txt", "r");
						if(vm == NULL) {
							DriverError(1, "Virtual Machine did not produce valid output/trace, or error reading vmOut.txt");
							break;
						}
						while(fgets(string, 100, vm)) {
							printf("%s", string);
						}
						break;
					default:
						printf("%s\n", "Directive selection not found");
						break;
				}

				// here we compile and execute 1,2, or 3 of the options
			}
			else
				track++;
		}
	}
}


int main(int argc, char **argv) {


	// valid (static) directive options able to be passed by user at command line
	char **directives = NULL;
	directives = malloc(sizeof(char*) * 3);
	directives[0] = lexemeList;
	directives[1] = generatedAssembly;
	directives[2] = vmExecutionTrace;

	 // buffer for printing lines
	char string[100];
	int checker = 1;
	int flag = 0;
	int q, z;
	char c;
	char s[20];
	//get input
	printf("Enter text file name to compile \n");
	scanf("%s",s);
	printf("\n");
	// check for valid input file
	FILE *checkInput = fopen(s, "r");
	if(checkInput == NULL) {
		DriverError(2, "No input.txt text file found, or fatal error reading from file.");
		fclose(checkInput);
		FILE *create = fopen(s, "w");
		// can't create an input.txt file for running? Something is seriously wrong. Abort.
		if(create == NULL) {
			DriverError(1, "Could not create input.txt file, try checking the filesystem for 'input.txt'");
			return 1;
		}
		DriverError(3, "Successfully created an input.txt file as one was not present.\nPlease open it, enter parser input and save. Then try running the driver again.");
		fclose(create);
		return 1;
	}
	else {
		// check if input file is empty
   		fseek (checkInput, 0, SEEK_END);
    	checker = ftell(checkInput);
		if(checker == 0) {
			DriverError(4, "Input file (input.txt) is empty.\nPlease add code for parser, save, and attempt to run driver again.");
			return 1;
		}
		printf("%s\n", "Valid input file (input.txt) found. Attempting to execute directives ... \n");
		fclose(checkInput);
	}

	// compile related source files and run them
	system("gcc -o lexer lexer.c");
	system("gcc -o assemblyGen parser.c");
	system("gcc -o vm vm.c");
	char buf[32];
    sprintf(buf, "./lexer %s", s);
	system(buf);
	FILE *lex = fopen("lexOutTable.txt", "r");
	fscanf(lex,"%d",&flag);
	if(flag == 0)
	system("./assemblyGen");
	else{
	    fgets(string, 100, lex);
	    printf("Error %s", string);
	    return 0;
	}
	fclose(lex);
	FILE *ga = fopen("parserOut.txt", "r");
	fscanf(ga,"%d",&flag);
	if(flag == 0)
	system("./vm");
    else{
        z=getSymbols();
						for(q=0; q<z; q++){
						    if(tknlist[q].type==2 || tknlist[q].type==3)
                            printf("%d %s ",tknlist[q].type,tknlist[q].name);
                            else printf("%d ",tknlist[q].type);
						}
						printf("\n");
						for(q=0; q<z; q++){
						    if(tknlist[q].type==2 || tknlist[q].type==3)
                            printf("%s %s ",tokenstr[tknlist[q].type-1],tknlist[q].name);
                            else printf("%s ",tokenstr[tknlist[q].type-1]);
						}
						printf("\n\n");
	    while(!feof(ga)){
	     c = getc(ga);
	    printf("%c", c);}
	    return 0;
	}
	fclose(ga);
	// command line checking block, checks for valid inputs of directives
	if(argc > 4 || argc < 0) {
		DriverError(1, "Please enter a valid number of arguments (0-3).");
		return 1;
	}

	int i = 0;
	// check all directive lengths and format after executable name
	if(argc > 0) {
		for(i = 1; i < argc; i++) {
			if(strlen(argv[i]) != 2) {  // expects '-' and one letter (v, a, l)
				DriverError(1, "Directive input not expected length.");
				return 1;
			} // check for correct format
			if(argv[i][0] != '-') {
				DriverError(1, "Directive input missing flag [""-""] .");
				return 1;
			}
			// check if valid flag was used, else halt run
			if((argv[i][1] == 'l') || (argv[i][1] == 'a') || (argv[i][1] == 'v')) {
				continue;
			}
			else { // invalid letter follows '-'
				DriverError(1, "Invalid flag entered. ");
				return 1;
			}
		}
	}

	// no directives entered, simply prints IN and OUT
	// otherwise check which directives were entered and carry out related operations
	if(argc == 1) {
		printf("\n Executing ... [%s] Input: \n\n", "default");
		FILE *in = fopen(s, "r");
		if(in == NULL) {
			DriverError(1, "Input file does not exist, or error opening file");
		}
		else {
			while(fgets(string, 100, in)) {
				printf("%s", string);
			}
			fclose(in);
		}

		printf("\n");
		printf("\n Executing ... [%s] Output (Assembly): \n\n", "default");
		FILE *po = fopen("parserOut.txt", "r");
		if(po == NULL) {
			DriverError(1, "Parser file does not exist, or error opening file");
		}
		else {
		    fscanf(po,"%d",&q);
			while(fgets(string, 100, po)) {
				printf("%s", string);
			}
			fclose(po);
		}
		return 1;
	}
	else { // decodes directive count > 0 and executes each

		ExecuteDirectives(argc, directives, argv);
	}

	return 0;

}
