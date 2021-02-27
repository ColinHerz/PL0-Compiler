/*
	Colin Herzberg
	COP-3402 Assignment 2
	UCFID-c4037960
*/

#define MAX_LEX 1000
#define MAX_NAME_LEN 11
#define MAX_NUM_LEN 5
#define NUM_SYM 13
#define NUM_RES_WORD 14

typedef struct lexeme{
	char lex [MAX_NAME_LEN];
	int number;
	int token;
}lexeme;

lexeme tokenList[MAX_LEX];

typedef enum token_type{ 
	nulsym = 1, identsym = 2, numbersym = 3, 
	plussym = 4, minussym = 5, multsym = 6,
	slashsym = 7, oddsym = 8, eqlsym = 9,
	neqsym = 10, lessym = 11, leqsym = 12, 
	gtrsym = 13, geqsym = 14, lparentsym = 15,
	rparentsym = 16, commasym = 17, semicolonsym = 18,
	periodsym = 19, becomessym = 20, beginsym = 21,
	endsym = 22, ifsym = 23, thensym = 24,
	whilesym = 25, dosym = 26, callsym = 27, 
	constsym = 28, varsym = 29, procsym = 30, 
	writesym = 31, readsym = 32, elsesym = 33
}token_type;

char specialSymbols[] = {
						'+', '-', '*', '/', '(', ')',
						'=', ',', '.', '<', '>', ';',
						':'
						};
char* specialWords[] = {
					   "const", "var", "procedure",
					   "call", "begin", "end",
					   "if", "then", "else",
					   "while", "do", "read",
					   "write", "odd"
					   };
char* errorList[] = {
					"Error 26: Identifier Too Long\n",
					"Error 25: This Number Is Too Large\n",
					"Error 27: Variable Must Start With Letter\n",
					"Error 28: Invalid Symbol\n"
					};
					   


void lexscan(FILE* in)
{
	char nextChar;
	int counter = 0;
	int tokenCounter = 0;
	int errFlag = 0;
	int switchFlag = 0;
	int i;
	
	nextChar = fgetc(in);
	while(nextChar != EOF)
	{
		
		// check if space
		if(nextChar == ' ' || nextChar =='\t' || nextChar == '\n' || nextChar == '\r')
		{
			// If it is white space, we should ignore it
			nextChar = fgetc(in);
			counter = 0;
			continue;			
		}
		
		// check if letter
		if(isalpha(nextChar))
		{
			// making space for the word that were going to read in
			char curString [MAX_NAME_LEN]; 
			memset(curString, 0, sizeof(curString));
			int nameLen = 0;
			
			curString[nameLen++] = nextChar;
			
			counter = 1;
			nextChar = fgetc(in);
			
			while(isalpha(nextChar) || isdigit(nextChar))
			{
				// scan in the rest of the word
				if(nameLen >= MAX_NAME_LEN)
				{
					// if its to long set the error flag and keep moving
					errFlag = 1;
				}
				else
				{
					curString[nameLen++] = nextChar;
				}
				nextChar = fgetc(in);
			}
			
			if(errFlag != 0) 
			{
				printf("%s", errorList[errFlag-1]);
				exit(1);
				errFlag=0;
                continue;
            }
			
			// check if reserved word
			switchFlag = -1;
			for(i = 0; i < NUM_RES_WORD - 1; i++)
			{
				if(strcmp(curString, specialWords[i]) == 0)
				{
					// yay its a reserved word
					switchFlag = i;
					break;
				}
			}
			
			// setting the current token list item to the enum for it
			switch(switchFlag)
			{
                case 0:
                    tokenList[tokenCounter].token = constsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 1:
                    tokenList[tokenCounter].token = varsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 2:
                    tokenList[tokenCounter].token = procsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 3:
                    tokenList[tokenCounter].token = callsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 4:
                    tokenList[tokenCounter].token = beginsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 5:
                    tokenList[tokenCounter].token = endsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 6:
                    tokenList[tokenCounter].token = ifsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 7:
                    tokenList[tokenCounter].token = thensym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 8:
                    tokenList[tokenCounter].token = elsesym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 9:
                    tokenList[tokenCounter].token = whilesym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 10:
                    tokenList[tokenCounter].token = dosym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 11:
                    tokenList[tokenCounter].token = readsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 12:
                    tokenList[tokenCounter].token = writesym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                case 13:
                    tokenList[tokenCounter].token = oddsym;
					strcpy(tokenList[tokenCounter].lex, specialWords[switchFlag]);
                    break;
                default:
                    tokenList[tokenCounter].token = identsym;
                    strcpy(tokenList[tokenCounter].lex, curString);
                    break;
			}
			tokenCounter++;
		}
		else if(isdigit(nextChar))
		{
			// making an int to store the number in for future use
			int curNum = 0; 
			int numLen = 1;
			counter = 1;
			
			// setting the char to the int value for it
			curNum = nextChar - '0';
			
			nextChar = fgetc(in);
			while(isdigit(nextChar) || isalpha(nextChar))
			{
				if(numLen >= MAX_NUM_LEN)
				{
					errFlag = 2;
				}
				else if(isalpha(nextChar))
				{
					errFlag = 3;
				}
				else
				{
					// multiplying curNum by 10 to move it to the left one spot
					curNum = (curNum * 10) + (nextChar - '0');
					numLen++;
				}
				nextChar = fgetc(in);
			}
			
			if(errFlag != 0) 
			{
				printf("%s", errorList[errFlag-1]);
				exit(1);
				errFlag=0;
                continue;
            }
			
			tokenList[tokenCounter].token = numbersym;
            tokenList[tokenCounter++].number = curNum;
		}
		else
		{
			counter = 0;
			switchFlag = -1;
			for(i = 0; i < NUM_SYM; i++)
			{
				if(nextChar == specialSymbols[i])
				{
					switchFlag = i;
				}
			}
			
			switch(switchFlag)
			{
				case 0:
                    tokenList[tokenCounter].token = plussym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
                    break;
				case 1:
					tokenList[tokenCounter].token = minussym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
                    break;
				case 2:
					tokenList[tokenCounter].token = multsym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
                    break;
				case 3:
					// need to check if this slash is for comment or not
					nextChar = fgetc(in);
                    counter = 1;
                    if(nextChar == '*')
					{
                        // if * after / its a comment so we will walk though it
						int commentFlag=1;
                        counter = 0;
                        nextChar = fgetc(in);
                        while(commentFlag == 1)
						{
                            if(nextChar == '*')
							{
                                nextChar = fgetc(in);
                                if(nextChar == '/')
								{
                                    // slash to end the comment
									commentFlag=0;
                                }
                            }
                            else
							{
                                nextChar=fgetc(in);
                            }
                        }
                    }
                    else
					{
                        tokenList[tokenCounter].token = slashsym;
						tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
                    }
                    break;
				case 4:
					tokenList[tokenCounter].token = lparentsym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
					break;
				case 5:
					tokenList[tokenCounter].token = rparentsym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
					break;
				case 6:
					// in pl/0 this one is used for comparing
					tokenList[tokenCounter].token = eqlsym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
					break;
				case 7:
					tokenList[tokenCounter].token = commasym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
					break;
				case 8:
					tokenList[tokenCounter].token = periodsym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
					break;
				case 9:
					// need to check for <> <= or else its <
					nextChar = fgetc(in);
                    counter=1;
                    if(nextChar == '>')
					{
                        // in pl/0 this is !=
						tokenList[tokenCounter].token = neqsym;
						tokenList[tokenCounter].lex[0] = '<';
						tokenList[tokenCounter++].lex[1] = '>';
                        counter = 0;
                    }
                    else if(nextChar == '=')
					{
                        tokenList[tokenCounter].token = leqsym;
						tokenList[tokenCounter].lex[0] = '<';
						tokenList[tokenCounter++].lex[1] = '=';
                        counter = 0;
                    }
                    else
					{
                        tokenList[tokenCounter].token = lessym;
						tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
                    }
                    break;
				case 10:
					// need to check for >= or its >
					nextChar = fgetc(in);
                    counter=1;
					if(nextChar == '=')
					{
                        tokenList[tokenCounter].token = geqsym;
						tokenList[tokenCounter].lex[0] = '>';
						tokenList[tokenCounter++].lex[1] = '=';
                        counter=0;
                    }
                    else{
                        tokenList[tokenCounter].token = gtrsym;
						tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
                    }
                    break;
				case 11:
					tokenList[tokenCounter].token = semicolonsym;
					tokenList[tokenCounter++].lex[0] = specialSymbols[switchFlag];
					break;
				case 12:
					// need to check for := for setting a variable
                    nextChar = fgetc(in);
                    if(nextChar == '=')
					{
                        tokenList[tokenCounter].token = becomessym;
                        tokenList[tokenCounter].lex[0] = ':';
						tokenList[tokenCounter++].lex[1] = '=';
                    }
					else
					{
						printf("%s", errorList[3]);
						exit(1);
					}
					break;
                default:
						printf("%s", errorList[3]);
						exit(1);
                    break;
			}
			
			// if the counter is at 0, were not are not looking ahead
			// this means we must read in the next char
			// if it is not 0, we have already read it in, and can keep looping
			if(counter == 0)
			{
				nextChar=fgetc(in);
			}
		}
	}
}

void printSource(char* inputfile, FILE* out)
{
	FILE* in = fopen(inputfile, "r"); 
	fprintf(out, "Source Program:\n");
	
	char nextChar = fgetc(in);

	while(nextChar != EOF)
	{
		fprintf(out, "%c", nextChar);
		nextChar = fgetc(in);
	}
	fclose(in);
}

void printSourceConsole(char* inputfile)
{
	FILE* in = fopen(inputfile, "r"); 
	printf("Source Program:\n");
	
	char nextChar = fgetc(in);

	while(nextChar != EOF)
	{
		printf("%c", nextChar);
		nextChar = fgetc(in);
	}
	fclose(in);
}

void lexOutput(FILE* out)
{
	int i;
	// printing table
	fprintf(out, "Lexeme Table:\n");
	fprintf(out, "lexeme\t");
	fprintf(out, "token type\n");
	for(i = 0; i < MAX_LEX; i++)
	{
		if (tokenList[i].token != 0)
		{
			if(tokenList[i].number != 0)
			{
				fprintf(out, "%d   \t", tokenList[i].number);
				fprintf(out, "%d   \n", tokenList[i].token);
			}
			else
			{
				fprintf(out, "%s   \t", tokenList[i].lex);
				fprintf(out, "%d   \n", tokenList[i].token);
			}
		}
	}
	// printing list
	fprintf(out, "\nLexeme List:\n");
	for(i = 0; i < MAX_LEX; i++)
	{
		if (tokenList[i].token != 0)
		{
			if(tokenList[i].token == 2)
			{
				fprintf(out, "%d ", tokenList[i].token);
				fprintf(out, "%s ", tokenList[i].lex);
			}
			else if(tokenList[i].token == 3)
			{
				fprintf(out, "%d ", tokenList[i].token);
				fprintf(out, "%d ", tokenList[i].number);
			}
			else
			{
				fprintf(out, "%d ", tokenList[i].token);
			}
		}
	}
	fprintf(out, "\n");
}

void lexOutputConsole()
{
	int i;
	// printing table
	printf("Lexeme Table:\n");
	printf("lexeme\t");
	printf("token type\n");
	for(i = 0; i < MAX_LEX; i++)
	{
		if (tokenList[i].token != 0)
		{
			if(tokenList[i].number != 0)
			{
				printf("%d   \t", tokenList[i].number);
				printf("%d   \n", tokenList[i].token);
			}
			else
			{
				printf("%s   \t", tokenList[i].lex);
				printf("%d   \n", tokenList[i].token);
			}
		}
	}
	// printing list
	printf("\nLexeme List:\n");
	for(i = 0; i < MAX_LEX; i++)
	{
		if (tokenList[i].token != 0)
		{
			if(tokenList[i].token == 2)
			{
				printf("%d ", tokenList[i].token);
				printf("%s ", tokenList[i].lex);
			}
			else if(tokenList[i].token == 3)
			{
				printf("%d ", tokenList[i].token);
				printf("%d ", tokenList[i].number);
			}
			else
			{
				printf("%d ", tokenList[i].token);
			}
		}
	}
	printf("\n");
}