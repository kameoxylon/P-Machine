//Yitzak Hernandez

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define SIZE 500

int symbolKind, stackPointer = 4, registerPointer = 0, instructionCounter = 0;
int lexLevel = 0;
char symbolName[11], symbolValue[11], zero[1] = {'0'};

struct symbol
{
	int kind;			//const = 1, var = 2, proc = 3
	char name[11]; 		//name up to 11 chars
	int val; 			//number in ASCII 
	int level; 			//L level
	int addr; 			//M address
};

//Hash functions, instruction insert and symbol table.
int search(char name[]);
int hashCode (char name[]);
void insert (struct symbol symbol);
void getToken (FILE *fpLexemeList, char *buffer);
void createSymbol (int kind, char* name, char* val);
void instructionInsert (FILE* fpInstructions, int op, int r, int l, int m);
struct symbol* hashArray[SIZE];
struct symbol* newEntry;

//Parser
void errorMessage (int i);
int isNumberValid (char* buffer);
void program(FILE *fpInstructons, FILE* fpLexemeList);
int isIdentifierValid (char* buffer);
void term (FILE* fpInstructions, FILE* fpLexemeList, char* buffer);
void block (FILE* fpInstructions, FILE* fpLexemeList, char* buffer);
void factor (FILE* fpInstructions, FILE* fpLexemeList, char* buffer);
void statement (FILE* fpInstructions, FILE* fpLexemeList, char* buffer);
void condition (FILE* fpInstructions, FILE* fpLexemeList, char *buffer);
void expression (FILE* fpInstructions, FILE* fpLexemeList, char* buffer);

void parser ()
{
	FILE *fpLexemeList;
	fpLexemeList = fopen("lexemeList.txt", "r");

	FILE *fpInstructions;
	fpInstructions = fopen("instructions.txt", "w");

	if(fpLexemeList == NULL)
	{
		printf("ERROR LEXEME LIST FILE DOES NOT EXIST.\n");
	}
	else
	{
		program(fpInstructions, fpLexemeList);
		instructionInsert(fpInstructions, 11, 0, 0, 3);
		printf("\nNo errors program is syntactically correct!\n");

		fclose(fpLexemeList);
		fclose(fpInstructions);
	}
}

/********** Symbol table and hash function ***************************/
void createSymbol (int kind, char* name, char* val)
{
	struct symbol newSymbol;
	
	newSymbol.kind = kind;
	strcpy(newSymbol.name, name);
	sscanf(val, "%d", &newSymbol.val);

	insert(newSymbol);
}

void insert (struct symbol symbol)
{
	struct symbol *newEntry = (struct symbol*) malloc (sizeof(struct symbol));
	newEntry->kind = symbol.kind;
	strcpy(newEntry->name, symbol.name);
	newEntry->val = symbol.val;
	newEntry->level = 0;
	
	int hashIndex = hashCode(newEntry->name);

	//If its not in the hashtable add it
	if (search(newEntry->name) == -1)
	{
		while(hashArray[hashIndex] != NULL)
		{
			hashIndex++;

			hashIndex %= SIZE;
		}

		hashArray[hashIndex] = newEntry;
	}	
}

int hashCode (char name[])
{
	int i = (int)name[0];
	int j = (int)name[strlen(name)];
	return (i + j) % 17;
}

int search(char name[])
{
	int hashIndex = hashCode(name);

	while(hashArray[hashIndex] != NULL)
	{
		if (0 == strncmp(name, hashArray[hashIndex]->name, strlen(name)))
			return hashIndex;

		hashIndex++;
		hashIndex %= SIZE;
	}

	return -1;
}

int symType(char name[])
{
	int position = search(name);

	return hashArray[position]->kind;
}

int symLevel(char name[])
{
	int position = search(name);

	return hashArray[position]->level;
}

int symAddress(char name[])
{
	int position = search(name);

	return hashArray[position]->addr;
}

void instructionInsert (FILE* fpInstructions, int op, int r, int l, int m)
{
	instructionCounter++;

	if (registerPointer > 16)
	{
		errorMessage(29);
	}
	if (r == -1)
	{
		r = registerPointer;
		registerPointer++;
	}

	fprintf(fpInstructions, "%i %i %i %i\n", op, r, l, m);
}
/*********************************************************************/

/********************** Parser ***************************************/

void getToken (FILE *fpLexemeList, char *buffer)
{
	memset(buffer, '\0', sizeof(buffer));

	char ch;
	ch = fgetc(fpLexemeList);
	int i = 0, chi = ch;


	while((chi != -1) && (chi != 32))
	{
		printf("%c", ch);
		buffer[i] = ch;
		i++;
		ch = fgetc(fpLexemeList);	
		chi = ch;
	}

	printf("%c", ch);

	buffer[i] = '\0';
}

void program (FILE *fpInstructions, FILE *fpLexemeList)
{	
	char buffer[11];

	getToken(fpLexemeList, buffer);
	instructionInsert(fpInstructions, 6, 0, 0, stackPointer);
	instructionInsert(fpInstructions, 6, 0, 0, 1);
	stackPointer++;
	block(fpInstructions, fpLexemeList, buffer);

	//if buffer == "."
	if (0 == strncmp(buffer, "19", 2))
	{
		//do nothing		
	}
	else
	{
		errorMessage(9);
	}
	
}

void block (FILE* fpInstructions, FILE *fpLexemeList, char *buffer)
{
	//if constsym == true
	if (0 == strncmp(buffer, "28", 2))
	{	
		symbolKind = 1;

		//while we do not hit a comma continue
		do
		{
			//if token == identsym
			getToken(fpLexemeList, buffer);
			if (0 == strncmp(buffer, "2", 2))
			{
				//check if identifier is valid
				getToken(fpLexemeList, buffer);
				if (isIdentifierValid(buffer))
				{
					strcpy(symbolName, buffer);
				}
				else
				{
					errorMessage(27);
				}
			}
			else
			{
				errorMessage(4);
			}

			//if token == eqsym
			getToken(fpLexemeList, buffer);
			if (0 == strncmp(buffer, "9", 2))
			{
				//do nothing
			}
			// := instead of =
			else if (0 == strncmp(buffer, "20", 2))
			{
				errorMessage(1);
			}
			else
			{
				errorMessage(3);
			}

			//if token == number
			getToken(fpLexemeList, buffer);
			if (0 == strncmp(buffer, "3", 2))
			{
				//check if number is valid
				getToken(fpLexemeList, buffer);
				if (isNumberValid(buffer))
				{
					strcpy(symbolValue, buffer);
				}
				else
				{
					errorMessage(28);
				}
			}
			else
			{
				errorMessage(2);
			}

			//add const to symbol table.
			createSymbol(symbolKind, symbolName, symbolValue);

			//commasym check
			getToken(fpLexemeList, buffer);
		} while (0 == strncmp(buffer, "17", 2));
		
		//if token == semicolonsym
		if (0 == strncmp(buffer, "18", 2))
		{
			//do nothing
		}
		else if (0 == strncmp(buffer, "2", 2))
		{
			//forgot comma
			errorMessage(19);
		}
		else
		{
			//semicolon expected
			errorMessage(5);
		}
		
		getToken(fpLexemeList, buffer);
		block(fpInstructions, fpLexemeList, buffer);
	}
	//if token == varsym
	else if (0 == strncmp(buffer, "29", 2))
	{		
		symbolKind = 2;
		do
		{			
			//if token == identsym
			getToken(fpLexemeList, buffer);
		printf("buffer %s\n", buffer);
			
			if (0 == strncmp(buffer, "2", 2))
			{
				//check if identifier is valid
				getToken(fpLexemeList, buffer);
				if (isIdentifierValid(buffer))
				{
					strcpy(symbolName, buffer);
				}
				else
				{
					errorMessage(27);
				}
			}
			else
			{
				errorMessage(4);
			}

			//add vars to symbol table
			createSymbol(symbolKind, symbolName, zero);
			getToken(fpLexemeList, buffer);
		} while(0 == strncmp(buffer, "17", 2));

		//if token == semicolonsym
		if (0 == strncmp(buffer, "18", 2))
		{
			//do nothing
		}
		else if (0 == strncmp(buffer, "2", 2))
		{
			//forgot comma
			errorMessage(19);
		}
		else
		{
			//semicolon expected
			errorMessage(5);
		}

		getToken(fpLexemeList, buffer);
		block(fpInstructions, fpLexemeList, buffer);
	}
	//token == procedure
	else if (0 == strncmp(buffer, "30", 2))
	{
		symbolKind = 3;
		getToken(fpLexemeList, buffer);		

		//token == 2
		if (0 == strncmp(buffer, "2", 2))
		{
			//check if identifier is valid
			getToken(fpLexemeList, buffer);
			if (isIdentifierValid(buffer))
			{
				strcpy(symbolName, buffer);
			}
			else
			{
				errorMessage(27);
			}
		}
		else
		{
			errorMessage(4);
		}

		getToken(fpLexemeList, buffer);
		//if token == semicolonsym
		if (0 == strncmp(buffer, "18", 2))
		{
			//do nothing
		}
		else
		{
			//semicolon expected
			errorMessage(17); 
		}

		createSymbol(symbolKind, symbolName, zero);
		getToken(fpLexemeList, buffer);
		lexLevel++;
		instructionInsert(fpLexemeList, 7, 0, 0, 0 );
		block(fpInstructions, fpLexemeList, buffer);
		lexLevel--;

		//if token == semicolonsym
		if (0 == strncmp(buffer, "18", 2))
		{
			//do nothing
		}
		else
		{
			//semicolon expected
			errorMessage(17);
		}

		getToken(fpLexemeList, buffer);
	}

	statement(fpInstructions, fpLexemeList, buffer);
}

void statement (FILE *fpInstructions, FILE *fpLexemeList, char* buffer)
{
	//if token == identsym
	if (0 == strncmp(buffer, "2", 2))
	{
		//check if identifier is valid
		getToken(fpLexemeList, buffer);
		strcpy(symbolName, buffer);
		int temp = search(symbolName);
		if (isIdentifierValid(buffer))
		{
			if (temp < 0)
			{
				errorMessage(11);
			}
		}
		else
		{
			errorMessage(27);
		}

		//if token == becomessym
		getToken(fpLexemeList, buffer);
		if (0 == strncmp(buffer, "20", 2))
		{
			//do nothing
		}
		else
		{
			errorMessage(19);
		}

		getToken(fpLexemeList, buffer);
		expression(fpInstructions, fpLexemeList, buffer);
		hashArray[temp]->addr = stackPointer - 1;
		instructionInsert(fpInstructions, 4, (registerPointer - 1), 0, (stackPointer - 1));
		instructionInsert(fpInstructions, 6, 0, 0, 1);
		stackPointer++;
	}
	//if token == beginsym
	else if (0 == strncmp(buffer, "21", 2))
	{
		getToken(fpLexemeList, buffer);
		statement(fpInstructions, fpLexemeList, buffer);

		//do while semicolon
		while(0 == strncmp(buffer, "18", 2))
		{
			getToken(fpLexemeList, buffer);
			statement(fpInstructions, fpLexemeList, buffer);
		} 

		//if token == endsym
		if (0 == strncmp(buffer, "22", 2))
		{
			//do nothing
		}
		else
		{
			errorMessage(26);
		}

		getToken(fpLexemeList, buffer);
	}
	//if token == ifsym
	else if (0 == strncmp(buffer, "23", 2))
	{
		getToken(fpLexemeList, buffer);
		condition(fpInstructions, fpLexemeList, buffer);

		//if token == thensym
		if (0 == strncmp(buffer, "24", 2))
		{
			//do nothing
		}
		else
		{
			errorMessage(16);
		}

		instructionInsert(fpInstructions, 8, 0, 0, (instructionCounter + 3));
		getToken(fpLexemeList, buffer);
		statement(fpInstructions, fpLexemeList, buffer);
	}
	//if token == whilesym
	else if (0 == strncmp(buffer, "25", 2))
	{
		getToken(fpLexemeList, buffer);
		condition(fpInstructions, fpLexemeList, buffer);

		//if token == dosym
		if (0 == strncmp(buffer, "26", 2))
		{
			//do nothing
		}
		else
		{
			errorMessage(18);
		}

		getToken(fpLexemeList, buffer);
		statement(fpInstructions, fpLexemeList, buffer);
	}
	//if token == read
	else if (0 == strncmp(buffer, "32", 2))
	{
		//check for identifier
		getToken(fpLexemeList, buffer);
		if (0 == strncmp(buffer, "2", 2))
		{
			//check if identifier is valid
			getToken(fpLexemeList, buffer);
			strcpy(symbolName, buffer);
			int temp = search(symbolName);
			if (isIdentifierValid(buffer))
			{
				if (temp < 0)
				{
					errorMessage(11);
				}
				else if (hashArray[temp]->kind != 2)
				{
					errorMessage(12);
				}
				instructionInsert(fpInstructions, 10, -1, 0, 2);
			}
			else
			{
				errorMessage(27);
			}

			hashArray[temp]->addr = stackPointer - 1;
			instructionInsert(fpInstructions, 4, (registerPointer - 1), 0, (stackPointer - 1));
			instructionInsert(fpInstructions, 6, 0, 0, 1);
			stackPointer++;
		}
		//check for semicolon
		getToken(fpLexemeList, buffer);
		if (0 == strncmp(buffer, "18", 2))
		{
			//do nothing
		}
		else
		{
			errorMessage(5);
		}

		
		getToken(fpLexemeList, buffer);
		statement(fpInstructions, fpLexemeList, buffer);
	}
	//if token == write
	else if (0 == strncmp(buffer, "31", 2))
	{
		getToken(fpLexemeList, buffer);
		if (0 == strncmp(buffer, "2", 2))
		{
			//check if identifier is valid
			getToken(fpLexemeList, buffer);
			if (isIdentifierValid(buffer))
			{
				strcpy(symbolName, buffer);
				int temp = search(symbolName);
				if (temp < 0)
				{
					errorMessage(11);
				}
				instructionInsert(fpInstructions, 3, 0, 0, hashArray[temp]->addr);
			}
			else
			{
				errorMessage(27);
			}
		}

		getToken(fpLexemeList, buffer);
		instructionInsert(fpInstructions, 9, 0, 0, 1);
	}
}

void condition (FILE* fpInstructions, FILE* fpLexemeList, char* buffer)
{
	//if token == oddsym
	if (0 == strncmp(buffer, "8", 1))
	{
		getToken(fpLexemeList, buffer);
		expression(fpInstructions, fpLexemeList, buffer);

		instructionInsert(fpInstructions, 17, (registerPointer - 1), 0, 0);
	}
	else
	{
		expression(fpInstructions, fpLexemeList, buffer);

		//if token == relation
		if (0 == strncmp(buffer, "9", 2) || 0 == strncmp(buffer, "10", 2) || 
			0 == strncmp(buffer, "11", 2) || 0 == strncmp(buffer, "12", 2) || 
			0 == strncmp(buffer, "13", 2) || 0 == strncmp(buffer, "14", 2))
		{
			int operation;
			//equals
			if (0 == strncmp(buffer, "9", 1))
			{
				operation = 19;
			}
			//does not equal
			else if (0 == strncmp(buffer, "10", 2))
			{
				operation = 20;
			}
			//less than
			else if (0 == strncmp(buffer, "11", 2))
			{
				operation = 21;
			}
			//less than or equal
			else if (0 == strncmp(buffer, "12", 2))
			{
				operation = 22;
			}
			//greater than
			else if (0 == strncmp(buffer, "13", 2))
			{
				operation = 23;
			}
			//greater than or equal
			else
			{
				operation = 24;
			}

			getToken(fpLexemeList, buffer);
			term(fpInstructions, fpLexemeList, buffer);
			instructionInsert(fpInstructions, operation, (registerPointer - 2), (registerPointer - 2), (registerPointer - 1));
			registerPointer--;
		}
		else
		{
			errorMessage(20);
		}

		expression(fpInstructions, fpLexemeList, buffer);
	}
}

void expression (FILE* fpInstructions, FILE* fpLexemeList, char* buffer)
{	
	int operation = 0;
	//if token == plussym or minussym
	if (0 == strncmp(buffer, "4", 2) || 0 == strncmp(buffer, "5", 2))
	{	
		//save op for instruction insert
		if (0 == strncmp(buffer, "4", 2))
			operation = 13;
		else
			operation = 14;

		getToken(fpLexemeList, buffer);
		operation = 0;
	}

	term(fpInstructions, fpLexemeList, buffer);
	if (operation == 13)
	{
		instructionInsert(fpInstructions, operation, (registerPointer - 2), (registerPointer - 2), (registerPointer - 1));
		registerPointer--;
	}
	else if (operation == 14)
	{
		instructionInsert(fpInstructions, operation, (registerPointer - 2), (registerPointer - 2), (registerPointer - 1));
		registerPointer--;
	}
	operation = 0;

	while (0 == strncmp(buffer, "4", 2) || 0 == strncmp(buffer, "5", 2))
	{
		//save op for instruction insert
		if (0 == strncmp(buffer, "4", 2))
			operation = 13;
		else
			operation = 14;

		getToken(fpLexemeList, buffer);
		term(fpInstructions, fpLexemeList, buffer);

		if (operation == 13)
		{
			instructionInsert(fpInstructions, operation, (registerPointer - 2), (registerPointer - 2), (registerPointer - 1));
			registerPointer--;
		}
		else if (operation == 14)
		{
			instructionInsert(fpInstructions, operation, (registerPointer - 2), (registerPointer - 2), (registerPointer - 1));
			registerPointer--;
		}
	}
}

void term (FILE* fpInstructions, FILE* fpLexemeList, char* buffer)
{
	factor(fpInstructions, fpLexemeList, buffer);
	int operation = 0;

	//while token == multsym or slashsym
	while(0 == strncmp(buffer, "6", 2) || 0 == strncmp(buffer, "7", 2))
	{
		//save op for instruction insert
		if (0 == strncmp(buffer, "6", 2))
			operation = 15;
		else
			operation = 16;

		getToken(fpLexemeList, buffer);
		factor(fpInstructions, fpLexemeList, buffer);

		if (operation == 15)
			instructionInsert(fpInstructions, operation, (registerPointer - 2), (registerPointer - 2), (registerPointer - 1));
		else if (operation == 16)
			instructionInsert(fpInstructions, operation, (registerPointer - 2), (registerPointer - 2), (registerPointer - 1));
		registerPointer--;

	}
}

void factor (FILE* fpInstructions, FILE* fpLexemeList, char* buffer)
{
	//if token == identsym
	if (0 == strncmp(buffer, "2", 2))
	{
		//check if identifier is valid	
		getToken(fpLexemeList, buffer);	
		if (isIdentifierValid(buffer))
		{
			strcpy(symbolName, buffer);
			int temp = search(symbolName);
			if (temp < 0)
			{
				errorMessage(11);
			}

			instructionInsert(fpInstructions, 1, -1, 0, hashArray[temp]->val);
			getToken(fpLexemeList, buffer);
			temp = -1;
		}
		else
		{
			errorMessage(27);
		}
	}
	//if token == number
	else if (0 == strncmp(buffer, "3", 2))
	{
		//check if number is valid
		getToken(fpLexemeList, buffer);
		if (isNumberValid(buffer))
		{
			int numb;
			sscanf(buffer, "%d", &numb);
			instructionInsert(fpInstructions, 1, -1, 0, numb);
			numb = 0;
		}
		else
		{
			errorMessage(28);
		}

		getToken(fpLexemeList, buffer);
	}
	//if token == (
	else if (0 == strncmp(buffer, "15", 2))
	{
		getToken(fpLexemeList, buffer);
		expression(fpInstructions, fpLexemeList, buffer);

		//if token == )
		if (0 == strncmp(buffer, "16", 2))
		{
			
		}
		else
		{
			errorMessage(22);
		}

		getToken(fpLexemeList, buffer);
	}
}

int isIdentifierValid (char* buffer)
{
	int i = 1;

	if (isdigit(buffer[0]))
		return 0;

	while(isalpha(buffer[i]))
	{
		i++;
	}

	while(isdigit(buffer[i]))
	{
		i++;
	}

	if(buffer[i] == '\0')
		return 1;

	return 0;
}

int isNumberValid (char* buffer)
{
	int i = 1;

	if (!isdigit(buffer[0]))
		return 0;

	while(isdigit(buffer[i]))
	{
		i++;
	}

	if(buffer[i] == '\0')
		return 1;

	if (i < 6)
		errorMessage(25);
	return 0;
}

void errorMessage (int i)
{
	char error[29][60] = { "Use = instead of :=.\n", 					//1
			"= must be followed by a number.\n", 						//2
			"Identifier must be followed by =.\n", 						//3
			"const, var, procedure must be followed by identifier.\n",  //4
			"Semicolon or comma missing.\n", 							//5
			"Incorrect symbol after procedure declaration.\n",  		//6 /* not implemented */
			"Statement expected.\n",  									//7 /* not implemented */
			"Incorrect symbol after statement part in block.\n",  		//8 /* not implemented */
			"Period expected.\n", 										//9 
			"Semicolon between statements missing.\n", 					//10 /* not implemented */
			"Undeclared identifier.\n", 								//11 
			"Assignment to constant or procedure is not allowed.\n", 	//12 
			"Assignment operator expected.\n", 							//13 /* not implemented */
			"call must be followed by an identifier or number.\n", 		//14 /* not implemented */
			"Call of a constant or variable is meaningless.\n", 		//15 /* not implemented */
			"then expected.\n", 										//16
			"Semicolon or } expected.\n", 								//17 /* not implemented */
			"do expected.\n",											//18
			"Incorrect symbol following statement.\n", 					//19
			"Relational operator expected.\n",							//20
			"Expression must not contain a procedure identifier.\n", 	//21 /* not implemented */
			"Right parenthesis missing.\n",								//22
			"The preceding factor cannot begin with this symbol.\n", 	//23 /* not implemented */ 
			"An expression cannot begin with this symbol.\n",			//24 /* not implemented */
			"This number is too large.\n",								//25 /* taken care at lexical analyzer */
			"end expected.\n",											//26 
			"invalid identifier.", 										//27 /* taken care at lexical analyzer */
			"invalid number.", 											//28 /* taken care at lexical analyzer */
			"registers are full, unable to complete program" };			//29

	printf("Error %d - %s", i, error[i - 1]);	
	exit(EXIT_FAILURE);
}