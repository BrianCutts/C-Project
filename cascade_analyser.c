#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"cct_func.h"

Terms termVal; // struct to hold Terminal values

int circuitSize = 0;// variable to hold length of struct array

int line = 1; // variable to designate error location

int noOfNodes = 0;//variable to hold number of nodes in circuit


/** main function
* @return an integer, 0 on successful exit
*/
int main() {

	FILE *inFilePtr;// pointer for opening file to be read

	char *inFileName = (char *)malloc(sizeof(char) * 50); // string to hold input file name

	printf("Input File Name to be read:\n");

	fgets(inFileName, 50, stdin);

	inFileName[strcspn(inFileName, "\n")] = ' ';//removes the new line character from file name input to the console*/
	
	fopen_s(&inFilePtr, inFilename, "r"); // try to open a file (MSVC)

	if (inFilePtr == NULL)             // if open was unsuccessful
	{
		char command[50];

		strcpy_s(command, sizeof command, "dir");  // DOS/Windows instruction to list out name and contents of current directory

		system(command);		// windows/dos system should print out contents of current directory where program is looking for file

		perror("Error: ");		// prints description of the error

		return(-1);				// -1 indicates error
	}
	char *outFileName = (char *)malloc(sizeof(char) * 50); // string to hold output file name

	char *fileLine = (char *)malloc(sizeof(char) * 150); // string to hold each line of the file 

	char *firstLineOut = (char *)malloc(sizeof(char) * 150);// string to hold the first line of outputs to the file

	Outs *outArr = (Outs *)malloc(sizeof(Outs) * 10); // struct array to hold output values determined from OUTPUT block

	/*set an empty 2 by 2 array used to hold the final ABCD matrix*/
	double **ABCDmtrx = (double **)malloc(2 * sizeof(double*));

	for (int i = 0; i < 2; i++)
	{
		ABCDmtrx[i] = (double *)malloc(2 * sizeof(double));
	}

	find_circuit(inFilePtr, fileLine, line, &circuitSize, &noOfNodes);// discover size of the circuit and that the read in was valid

	find_terms(inFilePtr, fileLine, line, &termVal, &noOfNodes);//checks for the TERMS Delimiters and stores terminal values

	ImpMat *impArr = (ImpMat *)malloc(sizeof(ImpMat) * circuitSize);// struct array with known circuit size	

	find_n_store_circuit(inFilePtr, fileLine, line, impArr); // reread the circuit block with array of structs of proper size and store values

	find_output(inFilePtr, fileLine, line, outArr, outFileName);//checks for the OUTPUT Delimiters and stores outputs

	print_struct_array(impArr, circuitSize);

	qsort(impArr, circuitSize, sizeof(ImpMat), compare_nodes);// sort circuit elements based on nodes using compare_nodes comparator function for qsort

	print_struct_array(impArr, circuitSize);

	init_ABCD(ABCDmtrx, impArr);//set first element of structure array ABCD values to the ABCD 2x2 array

	calc_ABCD(ABCDmtrx, impArr, circuitSize);//	

	FILE *outputFilePtr;

	fopen_s(&outputFilePtr, outFileName, "w");
	if (outputFilePtr == NULL)             // i.e. an error occurred
	{
		char command[50];
		strcpy_s(command, sizeof command, "dir");  // DOS/Windows instruction to list out name and contents of current directory
		system(command);		// windows/dos system should print out contents of current directory where program is looking for file
		perror("Error: ");		// prints description of the error
		return(-1);				// -1 indicates error
	}

	calc_n_store_outputs(ABCDmtrx, &termVal, outArr);

	print_outputs(outArr, outputFilePtr);

	free(outArr);

	free(fileLine);

	free(outFileName);

	free(impArr);

	free(ABCDmtrx);

	fclose(outputFilePtr);

	fclose(inFilePtr);

	return 0;
}

