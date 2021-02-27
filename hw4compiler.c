/*
	Colin Herzberg
	COP-3402 Assignment 3 compile
	UCFID-c4037960
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"


FILE *in, *out;

int main(int argc, char *argv[])
{
	int i;
	int lflag;
	int aflag;
	int vflag;

	in = fopen(argv[1], "r");
    out = fopen(argv[2], "w");

	for(i = 3; i < argc; i++)
	{
		if(strcmp(argv[i], "-l") == 0)
		{
			lflag = 1;
		}
		if(strcmp(argv[i], "-a") == 0)
		{
			aflag = 1;
		}
		if(strcmp(argv[i], "-v") == 0)
		{
			vflag = 1;
		}
	}

	// printing source to file and possibly console
	printSource(argv[1], out);
	if(lflag == 1)
	{
		printSourceConsole(argv[1]);
	}
	fprintf(out, "\n");
	
	// calling the lexer
	lexscan(in);

	// printing the output (tokens) to file and possibly console
	lexOutput(out);
	fprintf(out, "\n");
	if(lflag == 1)
	{
		lexOutputConsole();
		printf("\n");
	}

	// Converting the numerical tokens to token form 
	// then printing to file and possibly console
	printTokensAsName(tokenList, out);
	if(aflag == 1)
	{
		printTokensAsNameConsole(tokenList);
	} 

	// Calling the Parser
	parserMain(out);
	printSymbols("this is after ", out);
	
	// If there are no errors the program will not have exited by now
	// so print out that the code is good to go for VM
	fprintf(out, "\nNo errors, program is syntactically correct \n\n");
	if(aflag == 1)
	{
		printf("\nNo errors, program is syntactically correct \n\n");
	}

	// Since the PL/0 code is correct, we have converted it to assembly
	// thus we can print that to file and possibly console too
	printGenOps(out);
	fprintf(out, "\n");
	if(aflag == 1)
	{
		printGenOpsConsole();
		printf("\n");
	}

	// copying the generated instructions into the instructions
	// array for the VM (I hate that I have done this, I
	// should have just pushed everything right into VM array
	// but I did'nt I might change this if I have time)
	i = 0;
	while(vmCode[i].op != 0)
    {
        instructions[i] = vmCode[i];
		i++;
    }
	int numInstructs = i;

	// printing the initial state and converted instructions
	outputInstructions(out, instructions, numInstructs);
	outputInitalState(out);

	if(vflag == 1)
	{
		outputInstructionsConsole(instructions, numInstructs);
		outputInitalStateConsole();
	}

	// calling the VM
	virtualmach(out, vflag);
}