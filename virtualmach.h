/*
	Colin Herzberg
	COP-3402 Assignment 1
	UCFID-c4037960
*/

#include <stdlib.h>
#include <stdio.h>

#define MAX_CODE_LENGTH 200
#define MAX_LEXI_LEVELS 3
#define MAX_DATA_STACK_HEIGHT 40
#define NUM_REGISTER 8

typedef struct Instruction {
int op;    	/* opcode */
int  r;		/* R  */
int  l;     /* L  */        
int  m;   	/* M  */     
}Instruction;

//Prototypes
void virtualmach();
Instruction* readInstructions(FILE* input, Instruction* instructions, int* len);
void outputInstructions(FILE* output, Instruction* instructions, int len);
void outputInstructionsConsole(Instruction* instructions, int len);
void outputInitalState(FILE* output);
void outputInitalStateConsole();
int base(int l, int base);
void printPC(int PC, FILE* output);
void printPCConsole(int PC);
void printInstruction(int PC, Instruction IR, FILE* output);
void printInstructionConsole(int PC, Instruction IR);
void printRegisters(int SP, int BP, int* RF, FILE* output);
void printRegistersConsole(int SP, int BP, int* RF);
void printStack(int SP, int* stack, FILE* output);
void printStackConsole(int SP, int* stack);
//----------

// Global variables, becasue im stupid
char* opToString[] = 
{ 				  
	"NULL",
	"LIT","RTN","LOD", 
	"STO","CAL","INC",
	"JMP","JPC","SIO",
	"SIO","SIO","NEG", 
	"ADD","SUB","MUL",
	"DIV","ODD","MOD",
	"EQL","NEQ","LSS",
	"LEQ","GTR","GEQ"
};

int stack[MAX_DATA_STACK_HEIGHT + 1];
int RF[NUM_REGISTER];
int ARs[MAX_DATA_STACK_HEIGHT];
int ARcount = 0;
int CALFLAG = 0;
int SP = 0; 
int BP = 1; 
int PC = 0;
int HALT = 0;
Instruction IR;
Instruction instructions[MAX_CODE_LENGTH];
//-------------------------------------
FILE *in; //, *out;

void virtualmach(FILE* out, int flag)
{
		
	// Making a variable for length of input
	int programLength = 0;
	
	// Loop through and execute until halt flag
	while(!HALT)
	{
		// Fetch instruction
		IR = instructions[PC];
		
		// Print the instruction to output
		printInstruction(PC, IR, out);
		if(flag == 1)
		{
			printInstructionConsole(PC, IR);
		}
		
		// Increment program counter
		PC++;
		
		// Execute instruction
		switch (IR.op)
		{
			case 1: 
				RF[IR.r] = IR.m;
				break;
			case 2: 
				SP = BP - 1;
				BP = stack[SP + 3];
				PC = stack[SP + 4];
				ARs[--ARcount] = 0;
				break;
			case 3:
				RF[IR.r] = stack[base(IR.l, BP) + IR.m];
				break;
			case 4: 
				stack[base(IR.l, BP) + IR.m] = RF[IR.r];
				break;
			case 5: 
				CALFLAG = 1;
				stack[SP + 1]  = 0;							// space to return value
				stack[SP + 2]  = base(IR.l, BP); 			// static link (SL)
				stack[SP + 3]  = BP;						// dynamic link (DL)
				stack[SP + 4]  = PC;	 					// return address (RA) 
				BP = SP + 1;
				PC = IR.m;
				break;
			case 6: 
				if(CALFLAG == 1)
				{
					ARs[ARcount++] = SP;
				}
				SP = SP + IR.m;
				break;
			case 7: 
				PC = IR.m;
				break;
			case 8: 
				if(RF[IR.r] == 0)
				{
					PC = IR.m;
				}
				break;
			case 12: 
				RF[IR.r] = (-1) * RF[IR.l];
				break;
			case 13: 
				RF[IR.r] = RF[IR.l] + RF[IR.m];
				break;
			case 14: 
				RF[IR.r] = RF[IR.l] - RF[IR.m];
				break;
			case 15: 
				RF[IR.r] = RF[IR.l] * RF[IR.m];
				break;
			case 16: 
				RF[IR.r] = RF[IR.l] / RF[IR.m];
				break;
			case 17: 
				RF[IR.r] = RF[IR.r] % 2;
				break;
			case 18: 
				RF[IR.r] = RF[IR.l] % RF[IR.m];
				break;
			case 19: 
				RF[IR.r] = RF[IR.l] == RF[IR.m];
				break;
			case 20: 
				RF[IR.r] = RF[IR.l] != RF[IR.m];
				break;
			case 21: 
				RF[IR.r] = RF[IR.l] < RF[IR.m];
				break;
			case 22: 
				RF[IR.r] = RF[IR.l] <= RF[IR.m];
				break;
			case 23: 
				RF[IR.r] = RF[IR.l] > RF[IR.m];
				break;
			case 24: 
				RF[IR.r] = RF[IR.l] >= RF[IR.m];
				break;		
			default: 
				switch (IR.m)
				{
					case 1:	
						printf("%d \n", RF[IR.r]);
						break;
					case 2: 
						scanf("%d", &RF[IR.r]);
						break;
					default: 
						HALT = 1;
				}
		}
		// Printing state after execute
		printPC(PC, out);
		printRegisters(SP, BP, RF, out);
		printStack(SP, stack, out);
		fprintf(out, "\n");
		if(flag == 1)
		{
			printPCConsole(PC);
			printRegistersConsole(SP, BP, RF);
			printStackConsole(SP, stack);
			printf("\n");
		}
	}	
}

Instruction* readInstructions(FILE* input, Instruction* instructions, int* len)
{
	int i = 0;
	while(!feof(input) && i < MAX_CODE_LENGTH)
	{
		fscanf(input,"%d %d %d %d", &instructions[i].op, &instructions[i].r, &instructions[i].l, &instructions[i].m);
		i++;
	}
	*len = i;
	return instructions;
}

void outputInstructions(FILE* output, Instruction* instructions, int len)
{
	int i = 0;
	
	fprintf(output, "Line OP R L M\n");
	while(i < len)
	{
		fprintf(output,"%d   %s %d %d %d\n", i, opToString[instructions[i].op], instructions[i].r, instructions[i].l, instructions[i].m);
		i++;
	}
	fprintf(output, "\n \n");
	fprintf(output, "----------------------------");
	fprintf(output, "\n \n");
	return;
}

void outputInstructionsConsole(Instruction* instructions, int len)
{
	int i = 0;
	
	printf("Line OP R L M\n");
	while(i < len)
	{
		printf("%d   %s %d %d %d\n", i, opToString[instructions[i].op], instructions[i].r, instructions[i].l, instructions[i].m);
		i++;
	}
	printf("\n \n");
	printf("----------------------------");
	printf("\n \n");
	return;
}

int base(int l, int base) // l stand for L in the instruction format
{  
  int b1; //find base L levels down
  b1 = base; 
  while (l > 0)
  {
    b1 = stack[b1 + 1];
    l--;
  }
  return b1;
}

void printPC(int PC, FILE* output)
{
	fprintf(output, "PC=%d\t", PC);
}

void printPCConsole(int PC)
{
	printf("PC=%d\t", PC);
}

void printInstruction(int PC,Instruction IR, FILE* output)
{
	fprintf(output, "%d %s %d %d %d \t", PC, opToString[IR.op], IR.r, IR.l, IR.m);
}

void printInstructionConsole(int PC,Instruction IR)
{
	printf("%d %s %d %d %d \t", PC, opToString[IR.op], IR.r, IR.l, IR.m);
}

void printRegisters(int SP,int BP,int* RF, FILE* output)
{
	int i = 0;
	fprintf(output, "BP=%d\tSP=%d\t", BP, SP);
	for(i = 0; i < NUM_REGISTER; i++)
	{
		fprintf(output, "%d ", RF[i]);
	}
	fprintf(output, "\n");
}

void printRegistersConsole(int SP,int BP,int* RF)
{
	int i = 0;
	printf("BP=%d\tSP=%d\t", BP, SP);
	for(i = 0; i < NUM_REGISTER; i++)
	{
		printf("%d ", RF[i]);
	}
	printf("\n");
}

void printStack(int SP, int* stack, FILE* output)
{
	int i = 0;
	int j = 0;
	fprintf(output, "Stack: ");
	for(i = 1; i <= SP; i++)
	{
		fprintf(output, "%d ", stack[i]);
		if(i == ARs[j])
		{
			j++;
			fprintf(output, "| ");
		}
	}
	fprintf(output, "\n");
}

void printStackConsole(int SP, int* stack)
{
	int i = 0;
	int j = 0;
	printf("Stack: ");
	for(i = 1; i <= SP; i++)
	{
		printf("%d ", stack[i]);
		if(i == ARs[j])
		{
			j++;
			printf("| ");
		}
	}
	printf("\n");
}

void outputInitalState(FILE* output)
{
	// Print inital state
	fprintf(output, "Inital values \t");
	printPC(PC, output);
	printRegisters(SP, BP, RF, output);
	printStack(MAX_DATA_STACK_HEIGHT, stack, output);
	fprintf(output, "\n");
}

void outputInitalStateConsole()
{
	// Print inital state
	printf("Inital values \t");
	printPCConsole(PC);
	printRegistersConsole(SP, BP, RF);
	printStackConsole(MAX_DATA_STACK_HEIGHT, stack);
	printf("\n");
}