//Yitzak Hernandez


#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

struct instruction{
	int op;
	int r;
	int l;
	int m;
};

void printAssembly (struct instruction inst, int counter, FILE * fp, int nl);
void printExecution (int stack[], int pc, int bp, int sp, int counter, int reg[], 
	struct instruction inst, FILE *fpExecution);
int readFile(FILE *fp, char *array);
int base(int l, int base, int *stack);	

void vm ()
{
	//Opens input file.
	FILE *fpInput;
	fpInput = fopen("instructions.txt", "r");

	char input[500];
	struct instruction  inst;
	int stack[2000], reg[16];
	int sp = 0,	bp = 1, pc = 0, in;
	int halt = 0, temp = 0, i = 1, j = 0, ch = 0;

	if (fpInput ==  NULL)
	{
		printf("ERROR READING FILE, FILE NAME DOES NOT EXIST");
	}
	else
	{
		//Creates assembly and execution files.
		FILE *fpAssembly;
		fpAssembly = fopen("assembly.txt", "w");
		FILE *fpExecution;
		fpExecution = fopen("execution.txt", "w");

		//Fills in array from input file.
		ch = readFile(fpInput, input);
		fclose(fpInput);

		//Here we print assembly, ch is simply input array size.
		temp = ch / 4;
		while (i <= temp)
		{
			inst.op = input[(i * 4) - 4];
			inst.r =  input[(i * 4) - 3];
			inst.l =  input[(i * 4) - 2];
			inst.m =  input[(i * 4) - 1];
			printAssembly(inst, i, fpAssembly, 1);

			i++;
		}
		fclose(fpAssembly);

		//This step we execute the code.
		while ((ch - (pc * 4) != 0) && halt == 0)
		{	
			pc++;

			//fill in our struct.
			for (j = 1; j <= 4; j++)
			{
				inst.op = input[(pc*4) - 4];
				inst.r = input[(pc*4) - 3];
				inst.l = input[(pc*4) - 2];
				inst.m = input[(pc*4) - 1];
			}
			

			if (inst.op == 1) // 0 = OP, 1 = R, 2 = L, 3 = inst.m
			{
				//LIT
				reg[inst.r] = inst.m;
			}
			else if (inst.op == 2)
			{
				//RTN
				sp = bp - 1;
				bp = stack[sp + 3];
				pc = stack[sp + 4];
			}
			else if (inst.op == 3)
			{
				//LOD
				reg[inst.r] = stack[ base(inst.l, bp, stack) + inst.m ];
			}
			else if (inst.op == 4)
			{
				//STO
				stack[ base(inst.l, bp, stack) + inst.m ] = reg[inst.r];
			}
			else if (inst.op == 5)
			{
				//CAL
				stack[sp + 1] = 0;
				stack[sp + 2] = base(inst.l, bp, stack);
				stack[sp + 3] = bp;
				stack[sp + 4] = pc;
				bp = sp + 1;
				pc = inst.m;
			}
			else if (inst.op == 6)
			{
				//INC
				sp = sp + inst.m;
			}
			else if (inst.op == 7)
			{
				//JMP
				pc = inst.m;
			}
			else if (inst.op == 8)
			{
				//JPC
				if (reg[inst.r] == 0)
					pc = inst.m;
			}
			else if (inst.op == 9)
			{
				//SIO
				printf("%i\n", reg[inst.r]);
			}
			else if (inst.op == 10)
			{
				//SIO
				printf("input: ");
				scanf("%i", &in);
				reg[inst.r] = in;
			}
			else if (inst.op == 11)
			{
				//SIO
				halt = 1;
			}
			else if (inst.op == 12)
			{
				//NEG
				reg[inst.r] = -reg[inst.l];
			}
			else if (inst.op == 13)
			{
				//ADD
				reg[inst.r] = reg[inst.l] + reg[inst.m];
			}
			else if (inst.op == 14)
			{
				//SUB
				reg[inst.r] = reg[inst.l] - reg[inst.m];
			}
			else if (inst.op == 15)
			{
				//MUL
				reg[inst.r] = reg[inst.l] * reg[inst.m];
			}
			else if (inst.op == 16)
			{
				//DIV
				reg[inst.r] = reg[inst.l] / reg[inst.m];
			}
			else if (inst.op == 17)
			{
				//ODD
				reg[inst.r] = (reg[inst.r] % 2);
			}
			else if (inst.op == 18)
			{
				//MOD
				reg[inst.r] = (reg[inst.l] % reg[inst.m]);
			}
			else if (inst.op == 19)
			{
				//EQL
				reg[inst.r] = (reg[inst.l] == reg[inst.m]);
			}
			else if (inst.op == 20)
			{
				//NEQ
				reg[inst.r] = (reg[inst.l] != reg[inst.m]);
			}
			else if (inst.op == 21)
			{
				//LSS
				reg[inst.r] = (reg[inst.l] < reg[inst.m]);
			}
			else if (inst.op == 22)
			{
				//LEQ
				reg[inst.r] = (reg[inst.l] <= reg[inst.m]);
			}
			else if (inst.op == 23)
			{
				///GTR
				reg[inst.r] = (reg[inst.l] > reg[inst.m]);
			}
			else if (inst.op == 24)
			{
				//GEQ
				reg[inst.r] = (reg[inst.l] >= reg[inst.m]);
			}
			else
			{
				printf("ERROR %i OPCODE DOES NOT EXIST\n", inst.op);
				halt = 1;
			}

			printExecution (stack, pc, bp, sp, (pc - 1), reg, inst, fpExecution);			
		}

		fclose(fpExecution);
	}
}

//Reads file into an array. Function makes sure only numbers get added into array.
//Returns size of array.
int readFile(FILE*fp, char *array)
{
	int i = 0, ch, temp;
	ch = fgetc(fp);

	while(ch >= 0)
	{
		temp = ch - 48;
		if(temp >= 0 && temp <= 10)
		{
			ch = fgetc(fp);
			if ((ch - 48) >= 0 && (ch - 48) <= 10)
			{
				temp = temp * 10 + (ch - 48);	
				ch = fgetc(fp);			
			}

			array[i] = temp;
			i++;
		}
		else 
		{
			ch = fgetc(fp);
		}
	}

	return i;
}

//Base fuction traverses through the activation records to the correct location you're lookin for.
int base(int l, int base, int *stack) // l stand for L in the instruction format
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

//Writes execution steps into a file called execution.txt
void printExecution (int stack[], int pc, int bp, int sp, int counter, int reg[],
	struct instruction inst, FILE *fpExecution)
{
	printAssembly(inst, counter, fpExecution, 0);

	int i;
	for (i = 0; i < (sp + 1); i++)
	{
		if (i < 1)
			fprintf(fpExecution, "\t%i\t%i\t%i", pc, bp, sp);
		
		fprintf(fpExecution, "\t%i", stack[i]);
	} 	

	fprintf(fpExecution, "\n");

	//Prints registers.
	for(i = 0; i < 16; i++)
		fprintf(fpExecution, "%i\t", reg[i]);
	
	fprintf(fpExecution, "\n");	
	fprintf(fpExecution, "\n");
}

//Writes assembly code into a file called assembly.txt
void printAssembly (struct instruction inst, int counter, FILE * fp, int nl)
{
	if (inst.op == 1) // 0 = OP, 1 = R, 2 = L, 3 = inst.m
		{
			//LIT
			fprintf(fp, "%i\tLIT\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 2)
		{
			//RTN
			fprintf(fp, "%i\tRTN\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 3)
		{
			//LOD
			fprintf(fp, "%i\tLOD\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 4)
		{
			//STO
			fprintf(fp, "%i\tSTO\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 5)
		{
			//CAL
			fprintf(fp, "%i\tCAL\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 6)
		{
			//INC
			fprintf(fp, "%i\tINC\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 7)
		{
			//JMP
			fprintf(fp, "%i\tJMP\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 8)
		{
			//JPC
			fprintf(fp, "%i\tJPC\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 9)
		{
			//SIO
			fprintf(fp, "%i\tSIO\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 10)
		{
			//SIO
			fprintf(fp, "%i\tSIO\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 11)
		{
			//SIO
			fprintf(fp, "%i\tSIO\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 12)
		{
			//NEG
			fprintf(fp, "%i\tNEG\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 13)
		{
			//ADD
			fprintf(fp, "%i\tADD\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 14)
		{
			//SUB
			fprintf(fp, "%i\tSUB\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 15)
		{
			//MUL
			fprintf(fp, "%i\tMUL\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 16)
		{
			//DIV
			fprintf(fp, "%i\tDIV\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 17)
		{
			//ODD
			fprintf(fp, "%i\tODD\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 18)
		{
			//MOD
			fprintf(fp, "%i\tMOD\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 19)
		{
			//EQL
			fprintf(fp, "%i\tEQL\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else if (inst.op == 20)
		{
			//NEQ
			fprintf(fp, "%i\tNEQ\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 21)
		{
			//LSS
			fprintf(fp, "%i\tLSS\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 22)
		{
			//LEQ
			fprintf(fp, "%i\tLEQ\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 23)
		{
			///GTR
			fprintf(fp, "%i\tGTR\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);

		}
	else if (inst.op == 24)
		{
			//GEQ
			fprintf(fp, "%i\tGEQ\t%i\t%i\t%i", counter, inst.r, inst.l, inst.m);
		}
	else
		{
			fprintf(fp, "%i IS NOT VALID OP!!\n", inst.op);
		}

	if (nl == 1)
		fprintf(fp, "\n");
}
