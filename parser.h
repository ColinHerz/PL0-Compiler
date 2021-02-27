/*
	Colin Herzberg
	COP-3402 Assignment 3 parser/gen
	UCFID-c4037960
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "lexscan.h"
#include "virtualmach.h"

#define MAX_SYMBOL_TABLE_SIZE 1000
#define MAX_LEX 1000
#define MAX_CODE_LENGTH 200

void parserMain(FILE *out);
void PARSEprogram(FILE *out);
void PARSEblock(int level, FILE *out);
void PARSEstatement(int level, FILE *out);
void PARSEcondition(int level, FILE *out);
void PARSEexpression(int level, FILE *out);
void PARSEterm(int level, FILE *out);
void PARSEfactor(int level, FILE *out);

void outputError(int errorCode, FILE *out);
void generate(int op, int l, int r, int m, FILE *out);
void getToken();
int peekToken();
void addSymbol(int kind, int level, int* dataAlocIndex);
void printTokensAsName(lexeme *lexlist, FILE *out);
void printTokensAsNameConsole(lexeme *lexlist);
int getSymbolPosition(int level);
void markActive(int index);
void printGenOps(FILE *out);
void printGenOpsConsole();
void printSymbols(char *where, FILE *out);
void printSymbolsConsole(char *where);

typedef struct symbol
{
	int kind;				 // const = 1, var = 2, proc = 3
	char name[MAX_NAME_LEN]; // name up to 11 chars
	int val;				 // number (ASCII value)
	int level;				 // L level
	int addr;				 // M address
	int mark;				 // to indicate that code has been generated already for a block
} symbol;

char *enumNames[] = {
	"", "nulsym", "identsym", "numbersym",
	"plussym", "minussym", "multsym",
	"slashsym", "oddsym", "eqlsym",
	"neqsym", "lessym", "leqsym",
	"gtrsym", "geqsym", "lparentsym",
	"rparentsym", "commasym", "semicolonsym",
	"periodsym", "becomessym", "beginsym",
	"endsym", "ifsym", "thensym",
	"whilesym", "dosym", "callsym",
	"constsym", "varsym", "procsym",
	"writesym", "readsym", "elsesym"};

char *parseErrorList[] = {
	"Use = instead of :=.",									 // 0
	"= must be followed by a number.",						 // 1
	"Identifier must be followed by =.",					 // 2
	"const, var, procedure must be followed by identifier.", // 3
	"Semicolon or comma missing.",							 // 4
	"Incorrect symbol after procedure declaration.",		 // 5
	"Statement expected.",									 // 6
	"Incorrect symbol after statement part in block.",		 // 7
	"Period expected.",										 // 8
	"Semicolon between statements missing.",				 // 9
	"Undeclared identifier.",								 // 10
	"Assignment to constant or procedure is not allowed.",	 // 11
	"Assignment operator expected.",						 // 12
	"call must be followed by an identifier.",				 // 13
	"call of a constant or variable is meaningless.",		 // 14
	"then expected.",										 // 15
	"Semicolon or end expected.",							 // 16
	"do expected.",											 // 17
	"Incorrect symbol following statement.",				 // 18
	"Relational operator expected.",						 // 19
	"Expression must not contain a procedure identifier.",	 // 20
	"Right parenthesis missing.",							 // 21
	"The preceding factor cannot begin with this symbol.",	 // 22
	"An expression cannot begin with this symbol.",			 // 23
	"This number is too large.",							 // 24
	"Code is too long"										 // 25
};

enum symbolType
{
	symbolConst = 1,
	symbolVar = 2
};

symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
int tableIndex = 0;
Instruction vmCode[MAX_CODE_LENGTH];
int instructionIndex = 0;
// token list declared in lex.h
int tokenIndex = 0;
// I'm using these to store current token info
int curToken;
char curTokenName[MAX_NAME_LEN];
int curTokenValue;
// Global data index that is reset when block is called
//int dataAlocIndex;
// Global Indexes for registers
int regIndex = 2;
int tempReg;

int curErrorCode = -1;

void parserMain(FILE *out)
{
	PARSEprogram(out);
}

void PARSEprogram(FILE *out)
{
	//printf("program\n");
	getToken();

	// calling block with an intial lex level of 0
	PARSEblock(0, out);

	if (curToken != periodsym)
	{
		//error
		outputError(8, out);
	}
	generate(11, 0, 0, 3, out);
}

void PARSEblock(int level, FILE *out)
{
	//printf("program\n");
	generate(7, 0, 0, 0, out);

	int initalTableIndex = tableIndex;
	int intitalCodeIndex = instructionIndex;
	int dataAlocIndex = 4;

	if (curToken == constsym)
	{
		do
		{
			getToken();
			if (curToken != identsym)
			{
				//error
				outputError(3, out);
			}
			getToken();
			if (curToken != eqlsym)
			{
				//error
				if (curToken == becomessym)
				{
					outputError(0, out);
				}
				else
				{
					outputError(2, out);
				}
			}
			getToken();
			if (curToken != numbersym)
			{
				//error
				outputError(1, out);
			}
			addSymbol(symbolConst, level, &dataAlocIndex);
			getToken();
		} while (curToken == commasym);

		if (curToken != semicolonsym)
		{
			//error
			outputError(4, out);
		}
		getToken();
	}

	if (curToken == varsym)
	{
		do
		{
			getToken();
			if (curToken != identsym)
			{
				//error
				outputError(3, out);
			}
			addSymbol(symbolVar, level, &dataAlocIndex);
			getToken();
		} while (curToken == commasym);

		if (curToken != semicolonsym)
		{
			//error
			outputError(4, out);
		}
		getToken();
	}
	while (curToken == procsym)
	{
		getToken();
		if (curToken != identsym)
		{
			//error
			outputError(5, out);
		}
		else
		{
			addSymbol(3, level, &dataAlocIndex);
		}
		getToken();
		if (curToken != semicolonsym)
		{
			//error
			outputError(9, out);
		}
		getToken();
		PARSEblock(level + 1, out); // parse block needs to be called sending data aloc index through again
		if (curToken != semicolonsym)
		{
			//error
			outputError(16, out);
		}		
		getToken();
	}
	// setting start address
	vmCode[symbolTable[initalTableIndex].addr].m = instructionIndex;
	symbolTable[initalTableIndex].addr = instructionIndex;
	intitalCodeIndex = instructionIndex; 
	generate(6, 0, 0, dataAlocIndex, out);
	PARSEstatement(level, out);
	if(level != 0)
	{
		generate(2, 0, 0, 0, out);
	}
}

void PARSEstatement(int level, FILE *out)
{
	//printf("statement\n");
	int codeIndex1 = instructionIndex;
	int codeIndex2 = instructionIndex;
	int tableLocation;

	if (curToken == identsym)
	{
		tableLocation = getSymbolPosition(level);
		if (tableLocation == -1)
		{
			outputError(10, out);
		}
		else if (symbolTable[tableLocation].kind != symbolVar)
		{
			outputError(11, out);
		}
		getToken();
		if (curToken != becomessym)
		{
			if (curToken == eqlsym)
			{
				outputError(0, out);
			}
			else
			{
				outputError(12, out);
			}
		}
		getToken();
		PARSEexpression(level, out);
		if (tableLocation != -1)
		{
			// store whats in register r at the vars address stored in the table, on the assigned lex level
			generate(4, level - symbolTable[tableLocation].level, tempReg, symbolTable[tableLocation].addr, out);
		}
	}
	else if (curToken == callsym)
	{
		getToken();
		if (curToken != identsym)
		{
			//Error
			outputError(13, out);
		}
		tableLocation = getSymbolPosition(level);
		printf("calling mark____________________________________________________________________________________________________\n");
		markActive(tableLocation);
		if(tableLocation == -1)
		{
			// symbol not found
			outputError(10, out); 
		}
		else if(symbolTable[tableLocation].kind != 3)
		{
			// if the kind is not 3 the identifier is not proc, so they are calling nothing lol
			outputError(14, out);
		}
		else
		{
			generate(5, level, 0, symbolTable[tableLocation].addr, out);
		}
		getToken();
	}
	else if (curToken == ifsym)
	{
		getToken();
		PARSEcondition(level, out);
		if (curToken == thensym)
		{
			// need if condition then, rather than {}
			getToken();
		}
		else
		{
			//printf("%d", curToken);
			outputError(15, out);
		}
		codeIndex1 = instructionIndex;
		generate(8, 0, tempReg, 0, out); // R jump conditional
		PARSEstatement(level, out);
		vmCode[codeIndex1].m = instructionIndex; // post the adress to jump over.
		if(peekToken() == elsesym)
		{
			// if the next token is else, we know that we have to add one to the jpc index to account for the next jump.
			vmCode[codeIndex1].m++;
		}
	}
	else if (curToken == elsesym)
	{
		getToken();
		codeIndex2 = instructionIndex;
		generate(7, 0, 0, 0, out); // this jump will jump over else.
		PARSEstatement(level, out);
		vmCode[codeIndex2].m = instructionIndex; // post the adress to jump
	}

	else if (curToken == beginsym)
	{
		getToken();
		PARSEstatement(level, out);
		while (curToken == semicolonsym)
		{
			// walk though statements until end
			getToken();
			PARSEstatement(level, out);
		}
		if (curToken != endsym)
		{
			outputError(18, out);
		}
		else
		{
			getToken();
		}
	}

	else if (curToken == whilesym)
	{
		codeIndex1 = instructionIndex;
		getToken();
		PARSEcondition(level, out);
		codeIndex2 = instructionIndex;
		generate(8, 0, tempReg, 0, out); // R JumpConditional
		if (curToken == dosym)
		{
			getToken();
		}
		else
		{
			outputError(17, out);
		}
		PARSEstatement(level, out);
		// jump back up to the start of the loop
		generate(7, 0, 0, codeIndex1, out);
		// move the jump to location to current index
		vmCode[codeIndex2].m = instructionIndex;
	}

	else if (curToken == readsym)
	{
		getToken();
		regIndex++;
		tempReg = regIndex % NUM_REGISTER;
		generate(10, 0, tempReg, 2, out); // R SIO Read
		tableLocation = getSymbolPosition(level);
		if (tableLocation == -1)
		{
			outputError(11, out);
		}
		else if (symbolTable[tableLocation].kind != symbolVar)
		{
			outputError(12, out);
		}
		// store what was read into the stack where the indentifiers address was
		generate(4, level - symbolTable[tableLocation].level, tempReg, symbolTable[tableLocation].addr, out); // R STO
		getToken();
	}

	else if (curToken == writesym)
	{
		getToken();
		PARSEexpression(level, out);
		generate(9, 0, tempReg, 1, out); // R SIO write
	}
}

void PARSEcondition(int level, FILE *out)
{
	//printf("condition\n");
	if (curToken == oddsym)
	{
		// Get the next token and start the expression
		getToken();
		PARSEexpression(level, out);
		regIndex++;
		tempReg = regIndex % NUM_REGISTER;
		generate(17, 0, tempReg, 0, out); // R ODD
	}
	else
	{
		PARSEexpression(level, out);
		// parse the expression then check for relation, if not, then error
		if (!(curToken == eqlsym || curToken == neqsym || curToken == lessym || curToken == leqsym || curToken == gtrsym || curToken == geqsym))
		{
			outputError(19, out);
		}
		else
		{
			int tempOperation = curToken;
			getToken();
			PARSEexpression(level, out);
			switch (tempOperation)
			{
			case eqlsym:
				regIndex++;
				tempReg = regIndex % NUM_REGISTER;
				generate(19, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out); // R j = l k =m
				break;
			case neqsym:
				regIndex++;
				tempReg = regIndex % NUM_REGISTER;
				generate(20, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out); // R
				break;
			case lessym:
				regIndex++;
				tempReg = regIndex % NUM_REGISTER;
				generate(21, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out); // R
				break;
			case leqsym:
				regIndex++;
				tempReg = regIndex % NUM_REGISTER;
				generate(22, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out); // R
				break;
			case gtrsym:
				regIndex++;
				tempReg = regIndex % NUM_REGISTER;
				generate(23, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out); // R
				break;
			case geqsym:
				regIndex++;
				tempReg = regIndex % NUM_REGISTER;
				generate(24, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out); // R
				break;
			default:
				outputError(19, out);
				break;
			}
		}
	}
}

void PARSEexpression(int level, FILE *out)
{
	//printf("expression\n");
	int tempOp;
	if (curToken == plussym || curToken == minussym)
	{
		tempOp = curToken;
		getToken();
		PARSEterm(level, out);
		if (tempOp = minussym)
		{
			regIndex++;
			tempReg = regIndex % NUM_REGISTER;
			generate(14, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out);
		}
	}
	else
	{
		PARSEterm(level, out);
	}
	while (curToken == plussym || curToken == minussym)
	{
		tempOp = curToken;
		getToken();
		PARSEterm(level, out);
		if (tempOp == minussym)
		{
			regIndex++;
			tempReg = regIndex % NUM_REGISTER;
			generate(14, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out);
		}
		else if (tempOp == plussym)
		{
			regIndex++;
			tempReg = regIndex % NUM_REGISTER;
			generate(13, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out);
		}
	}
}

void PARSEterm(int level, FILE *out)
{
	//printf("term\n");
	int tempOp;
	PARSEfactor(level, out);
	while (curToken == multsym || curToken == slashsym)
	{
		tempOp = curToken;
		getToken();
		PARSEfactor(level, out);
		if (tempOp == multsym)
		{
			regIndex++;
			tempReg = regIndex % NUM_REGISTER;
			generate(15, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out);
		}
		else if (tempOp == slashsym)
		{
			regIndex++;
			tempReg = regIndex % NUM_REGISTER;
			generate(16, (regIndex - 2) % NUM_REGISTER, tempReg, (regIndex - 1) % NUM_REGISTER, out);
		}
	}
}

void PARSEfactor(int level, FILE *out)
{
	//printf("factor\n");
	int tableLocation;

	while (curToken == identsym || curToken == numbersym || curToken == lparentsym)
	{
		if (curToken == identsym)
		{
			tableLocation = getSymbolPosition(level);
			if (tableLocation == -1)
			{
				outputError(11, out);
			}
			else
			{
				if (symbolTable[tableLocation].kind == symbolConst)
				{
					regIndex++;
					tempReg = regIndex % NUM_REGISTER;
					generate(1, 0, tempReg, symbolTable[tableLocation].val, out); // R LIT
				}
				else if (symbolTable[tableLocation].kind == symbolVar)
				{
					regIndex++;
					tempReg = regIndex % NUM_REGISTER;
					generate(3, level - symbolTable[tableLocation].level, tempReg, symbolTable[tableLocation].addr, out); // R LOD
				}
			}
			getToken();
		}
		else if (curToken == numbersym)
		{
			//printf("number\n");
			regIndex++;
			tempReg = regIndex % NUM_REGISTER;
			generate(1, 0, tempReg, curTokenValue, out); // R LIT
			getToken();
		}
		else if (curToken == lparentsym)
		{
			getToken();
			PARSEexpression(level, out);
			if (curToken == rparentsym)
			{
				getToken();
			}
			else
			{
				outputError(21, out);
			}
		}
	}
}

void outputError(int errorCode, FILE *out)
{
	fprintf(out, "\nError %d: %s\n", errorCode + 1, parseErrorList[errorCode]);
	exit(1);
}

void generate(int op, int l, int r, int m, FILE *out)
{
	// check if we are still valid lenght
	if (instructionIndex > MAX_CODE_LENGTH)
	{
		outputError(25, out);
	}
	// if we are, add the next instruction
	else
	{
		vmCode[instructionIndex].op = op; //opcode
		vmCode[instructionIndex].l = l;	  // lex level
		vmCode[instructionIndex].r = r;	  // register
		vmCode[instructionIndex].m = m;	  // modifier
		instructionIndex++;
	}
}

void getToken()
{
	// setting global token to current token
	curToken = tokenList[tokenIndex].token;

	// Checking if that token is identfifer to grab its name
	if (curToken == identsym)
	{
		strcpy(curTokenName, tokenList[tokenIndex].lex);
	}

	// Checking if token is number to grab its value
	else if (curToken == numbersym)
	{
		curTokenValue = tokenList[tokenIndex].number;
	}

	// iterating the global token index
	tokenIndex++;
}

int peekToken()
{
	return tokenList[tokenIndex].token;
}

void addSymbol(int kind, int level, int *dataAlocIndex)
{
	int i = tableIndex;
	tableIndex++;
	// storing the kind
	symbolTable[tableIndex].kind = kind;

	// storing the name
	strcpy(symbolTable[tableIndex].name, curTokenName);

	// store needed info for kind
	switch (kind)
	{
	case 1:
		// since its a constant, we need to store its value
		symbolTable[tableIndex].val = curTokenValue;
		symbolTable[tableIndex].mark = 0;
		break;
	case 2:
		symbolTable[tableIndex].level = level;
		symbolTable[tableIndex].addr = *dataAlocIndex;
		symbolTable[tableIndex].mark = 0;
		(*dataAlocIndex)++;
		break;
	case 3:
		symbolTable[tableIndex].level = level;
		symbolTable[tableIndex].mark = 0;
		if(level >= 1)
		{
			//printf("hey");
			while(symbolTable[i].kind != 3)
			{
				//printf("hi");
				symbolTable[i].mark = 1;
				i--;
			}
			symbolTable[i].mark = 1;
		}
	}
}

void printTokensAsName(lexeme *lexlist, FILE *out)
{
	int i;
	char tempWord[11];
	fprintf(out, "Converted Tokens:\n");
	for (i = 0; i < MAX_LEX; i++)
	{
		if (tokenList[i].token != 0)
		{
			if (tokenList[i].token == identsym)
			{
				fprintf(out, "%s ", enumNames[tokenList[i].token]);
				fprintf(out, "%s ", tokenList[i].lex);
			}
			else if (tokenList[i].token == numbersym)
			{
				fprintf(out, "%s ", enumNames[tokenList[i].token]);
				fprintf(out, "%d ", tokenList[i].number);
			}
			else
			{
				fprintf(out, "%s ", enumNames[tokenList[i].token]);
			}
		}
	}
	fprintf(out, "\n");
}

void printTokensAsNameConsole(lexeme *lexlist)
{
	int i;
	char tempWord[11];
	printf("Converted Tokens:\n");
	for (i = 0; i < MAX_LEX; i++)
	{
		if (tokenList[i].token != 0)
		{
			if (tokenList[i].token == identsym)
			{
				printf("%s ", enumNames[tokenList[i].token]);
				printf("%s ", tokenList[i].lex);
			}
			else if (tokenList[i].token == numbersym)
			{
				printf("%s ", enumNames[tokenList[i].token]);
				printf("%d ", tokenList[i].number);
			}
			else
			{
				printf("%s ", enumNames[tokenList[i].token]);
			}
		}
	}
	printf("\n");
}

int getSymbolPosition(int level)
{
	// we start with most recent index and look back, assuming we use recent variables sooner
	int localTableIndex = tableIndex;
	for (localTableIndex; localTableIndex > 0; localTableIndex--)
	{
		if (strcmp(symbolTable[localTableIndex].name, curTokenName) == 0)
		{
			if(level >= symbolTable[localTableIndex].level)
			{
				if(symbolTable[localTableIndex].mark == 0)
				{
					printf("found cur token %d %s index %d \n", curToken, curTokenName, localTableIndex);
					// only returns the index if the symbol is visable from this level
					return localTableIndex;
				}
				else{
					printf(" cur token %d %s index %d \n", curToken, curTokenName, localTableIndex);
					return localTableIndex;
					}
			}
		}
	}
	return -1; // error, symbol not found
}

void markActive(int index)
{
	index++;
	printf("ahg %d \n", index);
	printSymbolsConsole("hey");
	while(symbolTable[index].kind != 3 || symbolTable[index].addr == 0)
	{
		symbolTable[index].mark = 0;
		index++;
		printf("unmarking cur token %d %s index %d \n", curToken, curTokenName, index);
	}
}

void printGenOps(FILE *out)
{
	fprintf(out, "Generated Instructions:\n");
	int i = 0;
	while (vmCode[i].op != 0)
	{
		fprintf(out, " %d %d %d %d\n", vmCode[i].op, vmCode[i].r, vmCode[i].l, vmCode[i].m);
		i++;
	}
}

void printGenOpsConsole()
{
	printf("Generated Instructions:\n");
	int i = 0;
	while (vmCode[i].op != 0)
	{
		printf(" %d %d %d %d\n", vmCode[i].op, vmCode[i].r, vmCode[i].l, vmCode[i].m);
		i++;
	}
}

void printSymbols(char *where, FILE *out)
{
	int i = 1;
	fprintf(out, "%s %d \n", where, regIndex);
	while (!(symbolTable[i].addr == 0))
	{
		fprintf(out, "addr %d kind %d level %d name %s val %d mark %d\n", symbolTable[i].addr, symbolTable[i].kind, symbolTable[i].level, symbolTable[i].name, symbolTable[i].val, symbolTable[i].mark);
		i++;
	}
	fprintf(out, "\n");
}

void printSymbolsConsole(char *where)
{
	int i = 1;
	printf("%s %d \n", where, regIndex);
	while (!(symbolTable[i].addr == 0))
	{
		printf("addr %d kind %d level %d name %s val %d mark %d\n", symbolTable[i].addr, symbolTable[i].kind, symbolTable[i].level, symbolTable[i].name, symbolTable[i].val, symbolTable[i].mark);
		i++;
	}
	printf("\n");
}