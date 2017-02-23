//Yitzak Hernandez

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lexemeFile(FILE *fpInput);

struct symbol
{
	int kind;
	char name[11];
	int val;
	int level;
	int addr;
};
//Constants store: kind, name, value.
//Variables store: kind, name, L, M.
//Procedures store: kind, name, L, M.

enum tokenType
{
	nulsym = 1, identsym, numbersym, plussym, minussym, ///1 - 5
		multsym,  slashsym, oddsym, eqsym, neqsym, //6 - 10
		lessym, leqsym,	gtrsym, geqsym, lparentsym, //11 - 15
		rparentsym, commasym, semicolonsym, periodsym, becomessym, //16 - 20
		beginsym, endsym, ifsym, thensym, whilesym, //21 - 25
		dosym, callsym, constsym, varsym, procsym, //26 - 30
		writesym, readsym, elsesym //31 - 33
};

static char *reserveWord[13] = {"const", "var", "procedure", "call", "begin", "end",
	"if", "then", "else", "while", "do", "read", "write"};
static int reservedWordNumber[13] = {28, 29, 30, 27, 21, 22, 23, 24, 33, 25, 26, 32,
	31};

static char *specialSymbols[12] = {"+", "-", "*", "/", "(", ")", "=", ",", ".", "<",
	">", ";"};
static int specialSymbolsNumber[12] = {4, 5, 6, 7, 15, 16, 10, 17, 19, 11, 13, 18};

static char *remainingSymbols[6] = {"null", "odd", ":=", "#", "$", "%"};
static int remainingSymbolsNumber[6] = {1, 8, 20, 10, 12, 14};


void main (int argc, char *argv[]) 
{

	FILE *fpInput;
	fpInput = fopen(argv[1], "r");

	if (fpInput == NULL)
	{
		printf("ERROR READING THE FILE. FILE DOES NOT EXIST.\n");
	}
	else
	{
		int fileCharLength;

		fileCharLength = lexemeFile(fpInput);
		fclose(fpInput);
	}

}

int lexemeFile(FILE *fpInput)
{
	int temp, ltemp, charCounter = 0, possId = 0, possNum = 0, possSym = 0, i = 0;
	int numCounter = 0, symCounter = 0, halt = 0, genCounter = 0, commentState = 0;
	char ch, lch = '\0', buffer[15], bufferC[1];

	memset(buffer, '\0', 15);

	ch = tolower(fgetc(fpInput));
	lch = ch;
	ltemp = temp;

	FILE *fpLexemeTable;
	fpLexemeTable = fopen("lexemeTable.txt", "w");

	FILE *fpLexemeList;
	fpLexemeList = fopen("lexemeList.txt", "w");

	//We will go through the file from here. Two aspects to keep in mind while going through this.
	//ch holds the file pointer and the current character, buffer will hold the previous valid 
	//string up to where ch is pointing. ex- "var " buffer = "var", ch = " ".
	while(ch != EOF)
	{
		//Prints code to screen.
		printf("%c", ch);

		int rwLoc = -1, ssLoc = -1, rsLoc = -1;
		temp = ch;
		i++;

		if (temp == 42 && ltemp == 47)
		{
			//We are in comments. Do Nothing.
			commentState = 1;
		}
		else if (temp == 47 && ltemp == 42)
		{
			//We just left comments. Do Nothing.
			commentState = 0;
			memset(buffer, '\0', 15);
		}
		//If next character is a symbol and previous was a letter or a number (x, or *4)
		//We will take the buffer and save it to file, then save ch and save it on a separate line.
		else if (((((32 < temp) && (temp < 48 )) || ((57 < temp) && (temp < 65)) || 
			((90 < temp) && (temp < 97)) || ((122 < temp) && (temp < 127))) &&
			(((47 < ltemp) && (ltemp < 58)) || ((64 < ltemp) && (ltemp < 91)) || 
			((96 < ltemp) && (ltemp < 123))) || 
			(((32 < ltemp) && (ltemp < 48 )) || ((57 < ltemp) && (ltemp < 65)) || 
			((90 < ltemp) && (ltemp < 97)) || ((122 < ltemp) && (ltemp < 127))) &&
			(((47 < temp) && (temp < 58)) || ((64 < temp) && (temp < 91)) || 
			((96 < temp) && (temp < 123)))) && commentState == 0)
		{
			fprintf(fpLexemeTable, "%s ", buffer);

			//These three while loops checks to see if buffer holds a reserve word,
			//special character or anything else thats left.
			int j = 0;
			while (j < 13)
			{
				if (0 == strcmp(buffer, reserveWord[j]))
				{
					//this is a reserved word
					rwLoc = j;
				}

				j++;
			}

			j = 0;
			while (j  < 12)
			{
				if (0 == strcmp(buffer, specialSymbols[j]))
				{
					//This is a special character
					ssLoc = j;
				}

				j++;
			}

			j = 0;
			while (j  < 6)
			{
				if (0 == strcmp(buffer, remainingSymbols[j]))
				{
					//This is a special character
					rsLoc = j;
				}

				j++;
			}
			
			//Here we check if buffer is any of the reserved/special/etc string.
			if (rwLoc >= 0)
			{
				fprintf(fpLexemeTable, "%i\n", reservedWordNumber[rwLoc]);
				fprintf(fpLexemeList, "%i ", reservedWordNumber[rwLoc]);
			}
			else if (ssLoc >= 0)
			{	
				fprintf(fpLexemeTable, "%i\n", specialSymbolsNumber[ssLoc]);
				fprintf(fpLexemeList, "%i ", specialSymbolsNumber[ssLoc]);
			}
			else if (rsLoc >= 0)
			{
				fprintf(fpLexemeTable, "%i\n", remainingSymbolsNumber[rsLoc]);
				fprintf(fpLexemeList, "%i ", remainingSymbolsNumber[rsLoc]);
			}
			else if (possId == 1)
			{
				fprintf(fpLexemeTable, "%i\n", 2);
				fprintf(fpLexemeList, "%i ", 2);
				fprintf(fpLexemeList, "%s ", buffer);
			}
			else if (possNum == 1)
			{
				fprintf(fpLexemeTable, "%i\n", 3);
				fprintf(fpLexemeList, "%i ", 3);
				fprintf(fpLexemeList, "%s ", buffer);
			}
			else
			{
				printf("\nERROR UNIDENTIFIED CHARACTER IN FILE.\n");
				halt = 1;
			}

			//Resets everything.
			memset(buffer, '\0', 15);
			genCounter = 0;
			charCounter = 0;
			numCounter = 0;
			symCounter = 0;
			genCounter = 0;
			possId = 0;
			possSym = 0;
			possNum = 0;
			rwLoc = -1;
			ssLoc = -1;
			rsLoc = -1;

			temp = ch;

			if ((47 < temp) && (temp < 58))
			{ //Numerical Characters
				numCounter++;
				possNum = 1;

				buffer[genCounter] = ch;
				genCounter++;
			}
			else if (((64 < temp) && (temp < 91)) || ((96 < temp) && (temp < 123)))
			{ //Alphabet Characters
				charCounter++;
				possId = 1;

				//Check condition to see if variable starts with numbers.
				if ((47 < ltemp) && (ltemp < 58))
				{
					printf("\nVARIABLE DOES NOT START WITH A LETTER.\n");
					halt = 1;
				}

				buffer[genCounter] = ch;
				genCounter++;
			}
			else 
			{ //Special Characters
				symCounter++;
				possSym = 1;

				buffer[genCounter] = ch;
				genCounter++;
			}
		}
		else if (temp <= 32)
		{ //If we hit a control character we will check if buffer is empty. If its not,
			//we will add it to the output file.
			
			if (0 == strcmp(buffer, ""))
			{
				//We do nothing when we are going through a series of control characters.
			}
			else
			{
				fprintf(fpLexemeTable, "%s ", buffer);

				//These three while loops checks to see if buffer holds a reserve word,
				//special character or anything else thats left.
				int j = 0;
				while (j < 13)
				{
					if (0 == strcmp(buffer, reserveWord[j]))
					{
						//this is a reserved word
						rwLoc = j;
					}

					j++;
				}

				j = 0;
				while (j  < 12)
				{
					if (0 == strcmp(buffer, specialSymbols[j]))
					{
						//This is a special character
						ssLoc = j;
					}

					j++;
				}

				j = 0;
				while (j  < 6)
				{
					if (0 == strcmp(buffer, remainingSymbols[j]))
					{
						//This is a special character
						rsLoc = j;
					}

					j++;
				}

				//Here we check if buffer is any of the reserved/special/etc string.
				if (rwLoc >= 0)
				{
					fprintf(fpLexemeTable, "%i\n", reservedWordNumber[rwLoc]);
					fprintf(fpLexemeList, "%i ", reservedWordNumber[rwLoc]);
				}
				else if (ssLoc >= 0)
				{	
					fprintf(fpLexemeTable, "%i\n", specialSymbolsNumber[ssLoc]);
					fprintf(fpLexemeList, "%i ", specialSymbolsNumber[ssLoc]);
				}
				else if (rsLoc >= 0)
				{
					fprintf(fpLexemeTable, "%i\n", remainingSymbolsNumber[rsLoc]);
					fprintf(fpLexemeList, "%i ", remainingSymbolsNumber[rsLoc]);
				}
				else if (possId == 1)
				{
					fprintf(fpLexemeTable, "%i\n", 2);
					fprintf(fpLexemeList, "%i ", 2);
					fprintf(fpLexemeList, "%s ", buffer);
				}
				else if (possNum == 1)
				{
					fprintf(fpLexemeTable, "%i\n", 3);
					fprintf(fpLexemeList, "%i ", 3);
					fprintf(fpLexemeList, "%s ", buffer);
				}
				else
				{
					printf("\nERROR UNIDENTIFIED CHARACTER IN FILE.\n");
					halt = 1;
				}

				//Resets everything.
				charCounter = 0;
				numCounter = 0;
				symCounter = 0;
				genCounter = 0;
				possId = 0;
				possSym = 0;
				possNum = 0;
				rwLoc = -1;
				ssLoc = -1;
				rsLoc = -1;
			}

			//Resets Buffer.
			memset(buffer, '\0', 15);
		}
		else if ((47 < temp) && (temp < 58))
		{ //Numerical Characters
			numCounter++;
			possNum = 1;

			buffer[genCounter] = ch;
			genCounter++;
		}
		else if (((64 < temp) && (temp < 91)) || ((96 < temp) && (temp < 123)))
		{ //Alphabet Characters
			charCounter++;
			possId = 1;

			//Check condition to see if variable starts with numbers.
			if ((47 < ltemp) && (ltemp < 58))
			{
				printf("\nVARIABLE DOES NOT START WITH A LETTER.\n");
				halt = 1;
			}

			buffer[genCounter] = ch;
			genCounter++;
		}
		else if (((32 < temp) && (temp < 48 )) || ((57 < temp) && (temp < 65)) || 
			((90 < temp) && (temp < 97)) || ((122 < temp) && (temp < 127)))
		{ //Special Characters
			symCounter++;
			possSym = 1;

			buffer[genCounter] = ch;
			genCounter++;
		}
		else
		{
			printf("\nUNRECOGNIZED CHARACTER IN FILE.\n");
			halt = 1;
		}
		
		lch = ch;
		ltemp = temp;
		ch = tolower(fgetc(fpInput));
		
		//Check to make sure variable name isn't too long or that number isn't too large.
		if (charCounter > 12 || numCounter > 5)
		{
			printf("\nNUMBER TOO LARGE | VARIABLE NAME TOO LARGE %i\n", halt);
			printf("ERROR.\n");
			ch = EOF;
		}
		else if ( halt == 1 )
		{
			printf("ERROR.\n");
		}
	}

	fclose(fpLexemeTable);
	fclose(fpLexemeList);

	return i;
}