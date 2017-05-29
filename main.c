//Yitzak Hernandez 

#include <string.h>
#include <stdio.h>

int lex;
int parse;
int virtual;

void compile ();
void vm();
void parser();
void lexicalanalyzer();


int main (int argc, char *argv[])
{
	char** buffer = argv;
	FILE* fp;

	while (argc > 1)
	{
	    if (strcmp(buffer[1], "-l") == 0) 
	    {
	        lex = 1;
	    }
	    if (strcmp(buffer[1], "-a") == 0) 
	    {
	        parse = 1;
	    }
	    if (strcmp(buffer[1], "-v") == 0) 
	    {
	        virtual = 1;
	    }

	    argc --;
	    buffer ++;
	}

	compile();
}

void compile ()
{
	lexicalanalyzer();

	parser();

	vm();

	FILE *fp;
	char c;

	//prints lexememList to screen
	if (lex == 1)
	{
		fp = fopen("lexemeList.txt", "r");
		c = fgetc(fp);

		while(c != EOF)
		{
			printf("%c", c);
			c = fgetc(fp);
		}

		fclose(fp);
		printf("\n");
	}

	//prints assembly instructions to screen
	if (parse == 1)
	{

		fp = fopen("instructions.txt", "r");
		c = fgetc(fp);

		while(c != EOF)
		{
			printf("%c", c);
			c = fgetc(fp);
		}

		fclose(fp);
		printf("\n");
	}

	//Prints vm execution to screen
	if (virtual == 1)
	{
		fp = fopen("execution.txt", "r");
		c = fgetc(fp);

		while(c != EOF)
		{
			printf("%c", c);
			c = fgetc(fp);
		}

		fclose(fp);
		printf("\n");
	}
}