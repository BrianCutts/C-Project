#include"cct_func.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

void find_circuit(FILE *fileptr, char *circuitLine, int line, int *crt_sz, int *noOfNodes)
{
	//interate through file lines to determine CIRCUIT delimiters and circuit size
	while ((strstr(circuitLine, "</CIRCUIT>")) == NULL)
	{
		fgets(circuitLine, 150, fileptr);

		// if there is a blank line in the text file, skip and continue reading the file
		if (strcmp(circuitLine, "\n") == 0)
		{
			continue;
		}

		// if file line is a comment, skip
		if (strstr(circuitLine, "#"))
		{
			continue;
		}

		//once CIRCUIT block beginning delimiter is reached, determine number of component values
		if (strstr(circuitLine, "<CIRCUIT>"))
		{
			int no_of_comp = read_circuit_block(fileptr, circuitLine, line, noOfNodes);

			*crt_sz = no_of_comp;// set pointer *crt_sz (used in memory allocation) to the value obtained in read_circuit_block 

		}

		//else there is no CIRCUIT block		
		if (feof(fileptr))
		{
			printf("No CIRCUIT block in file. Exiting...");

			exit(-1);
		}
	}
}

int read_circuit_block(FILE *fileptr, char *circuitLine, int line, int *noOfNodes)
{
	//initialise local variables for the circuit read
	int n1, n2;

	int  noOfComps = 0, compReadCheck = 0, n1ReadCheck = 0, n2ReadCheck = 0;

	int offset = 2, nodes = 1;//offset used to index where the component value lies in the file line

	double compVal;

	char comp;

	while (1)
	{
		fgets(circuitLine, 150, fileptr);

		// if there is a blank line in the text file, skip and continue reading the file
		if (strcmp(circuitLine, "\n") == 0)//strcmp returns 0 for equality
		{
			continue;
		}

		// if comment line, skip
		if (strchr(circuitLine, '#'))
		{
			continue;
		}

		//if end delimiter is reached, break
		if (strstr(circuitLine, "</CIRCUIT>"))
		{
			break;
		}

		n1 = find_intVal(circuitLine, "n1", &n1ReadCheck); // get value for n1 and check for successful read

		n2 = find_intVal(circuitLine, "n2", &n2ReadCheck); // get value for n2 and check for successful read

		/*If component is found determine its value and
		increment the size of the circuit and the number of nodes*/
		if (strstr(circuitLine, "R="))
		{
			compVal = find_compVal(circuitLine, "R=", &compReadCheck, offset);

			comp = 'R';

			/*If either n1 or n2 are 0 only increment noOfComps*/
			if ((n1 == 0) || (n2 == 0))
			{
				noOfComps++;
			}

			//else increment both number of nodes and components
			else
			{
				noOfComps++;

				nodes++;

				*noOfNodes = nodes;
			}
		}

		/*If component is found determine its value and
		increment the size of the circuit and the number of nodes*/
		if (strstr(circuitLine, "G="))
		{
			compVal = find_compVal(circuitLine, "G=", &compReadCheck, offset);

			comp = 'G';

			/*If either n1 or n2 are 0 only increment noOfComps*/
			if ((n1 == 0) || (n2 == 0))
			{
				noOfComps++;
			}

			//else increment both number of nodes and components
			else
			{
				noOfComps++;

				nodes++;

				*noOfNodes = nodes;
			}
		}

		//check that the read was successful
		if (compReadCheck && n1ReadCheck && n2ReadCheck)
		{
			printf("Line %d: n1=%d, n2=%d, %c=%lf\n", line, n1, n2, comp, compVal);

		}

		else
		{
			printf("Read of line %d was unsucessful: %s \n", line, circuitLine);

			if (compVal < 0)
			{
				printf("Not a valid component value.\n");
			}

			if ((n1 < 0) || (n2 < 0))
			{
				printf("Not a valid node value.\n");
			}

			exit(-1);

		}

		line = line + 1;
	}

	return(noOfComps);// returns the integer value of the number of components/circuit size
}

void find_terms(FILE *fileptr, char *termsLine, int line, Terms *termVal, int *noOfNodes)
{
	// reset line read to beginning of the file and check for valid reset
	if (fseek(fileptr, 0, SEEK_SET) != 0)
	{
		char command[50];

		strcpy_s(command, sizeof command, "dir");  // DOS/Windows instruction to list out name and contents of current directory

		system(command);		// windows/dos system should print out contents of current directory where program is looking for file

		perror("Error: ");		// prints description of the error

		exit(-1);
	}

	while ((strstr(termsLine, "</TERMS>")) == NULL)
	{
		fgets(termsLine, 150, fileptr);// resets file pointer to next line in file

		// if there is a blank line in the text file, skip and continue reading the file
		if (strcmp(termsLine, "\n") == 0)
		{
			continue;
		}

		// if comment line, skip
		if (strstr(termsLine, "#"))
		{
			continue;
		}

		//once TERMS block beginning delimiter is reached, determine terminal values
		if (strstr(termsLine, "<TERMS>"))
		{
			read_terms_block(fileptr, termsLine, line, termVal, noOfNodes);
		}

		//if the end of the file is reached there is no TERMS block, exit the code
		if (feof(fileptr))
		{
			printf("No TERMS block in file. Exiting...");

			exit(-1);
		}
	}
}

void read_terms_block(FILE *fileptr, char *termsLine, int line, Terms *termVal, int *noOfNodes) {

	//initialise variable for successful read checks and component values
	int VT_read_OK = 0;

	int RS_read_OK = 0;

	int RL_read_OK = 0;

	int GS_read_OK = 0;

	int IN_read_OK = 0;

	double VT, RS, RL, GS, IN;

	int offset = 3, nodes = 0;// offset used to index where component value should be located

	while (1)
	{
		fgets(termsLine, 150, fileptr);

		// if there is a blank line in the text file, skip and continue reading the file
		if (strcmp(termsLine, "\n") == 0)
		{
			continue;
		}

		// if comment line, skip
		if (strstr(termsLine, "#"))
		{
			continue;
		}

		//if end delimiter is reached break
		if (strstr(termsLine, "</TERMS>"))
		{
			break;
		}

		//reads the value of Thevenin Voltage and Source Resistance
		if (strstr(termsLine, "VT"))
		{
			VT = find_compVal(termsLine, "VT", &VT_read_OK, offset);
		}

		//reads Norton current if present
		if (strstr(termsLine, "IN"))
		{
			IN = find_compVal(termsLine, "IN", &IN_read_OK, offset);
		}

		//reads line for RS value
		if (strstr(termsLine, "RS"))
		{
			RS = find_compVal(termsLine, "RS", &RS_read_OK, offset);

			GS_read_OK = 0;
		}
		//reads line for GS value
		if (strstr(termsLine, "GS"))
		{
			GS = find_compVal(termsLine, "GS", &GS_read_OK, offset);

			RS_read_OK = 0;
		}

		check_n_store_terms(termVal, termsLine, &VT, &RS, &IN, &GS, VT_read_OK, IN_read_OK, RS_read_OK, GS_read_OK, line);

		check_n_store_RL(termVal, termsLine, &RL, noOfNodes, &nodes, &RL_read_OK, offset, line);
		
	}
	line = line + 1;

}

void check_n_store_terms(Terms *termVal, char *termsLine, double *VT, double *RS, double *IN, double *GS, int VT_read_OK, int IN_read_OK, int RS_read_OK, int GS_read_OK, int line)
{
	// check that the read was successful for VT and RS
	if ((VT_read_OK) && (RS_read_OK))
	{
		printf("Read in 2 variables in Terms line %d, VT=%lf, RS=%lf \n", line, *VT, *RS);

		termVal->Vt = *VT;//store VT for Thevenin voltage in terms struct

		termVal->RS = *RS;//store source resistance RS in terms struct

	}

	//check for VT and GS read
	else if ((VT_read_OK) && (GS_read_OK))
	{
		termVal->Vt = *VT;//store VT for Thevenin voltage in terms struct

		*RS = (1 / *GS);//convert GS to series resistance

		printf("Read in 2 variables in Terms line %d, VT=%lf, RS=%lf \n", line, *VT, *RS);

		termVal->RS = *RS;//store GS as 1/GS for consistancy 
	}

	//check for IN and RS read
	else if ((IN_read_OK) && (RS_read_OK))
	{
		printf("Read in 2 variables in Terms line %d, IN=%lf, RS=%lf \n", line, *IN, *RS);

		termVal->In = *IN;//store IN in terms struct if Norton current

		termVal->RS = *RS;//store source resistance RS in terms struct
	}

	//check for IN and GS read
	else if ((IN_read_OK) && (GS_read_OK))
	{
		termVal->In = *IN;//store IN in terms struct if Norton current

		*RS = (1 / *GS);//convert GS to series resistance

		printf("Read in 2 variables in Terms line %d, IN=%lf, RS=%lf \n", line, *IN, *RS);

		termVal->RS = *RS;//store source resistance RS in terms struct
	}

	//otherwise not a valid read
	else
	{
		printf("Read of line %d failed : %s\nExiting...", line, termsLine);

		exit(-1);
	}
}

void check_n_store_RL(Terms *termVal, char *termsLine, double *RL, int *noOfNodes, int *nodes, int *RL_read_OK, int offset, int line)
{
	if (strstr(termsLine, "RL"))
	{

		// checks that the first element of the line associated with RL is a digit		
		if (isdigit(termsLine[0]))
		{
			*nodes = termsLine[0] - '0';//gets integer equivalent of that first digit

			// makes sure that first digit and the actual number of nodes in the circuit are equal
			if (*nodes == *noOfNodes)
			{
				*RL = find_compVal(termsLine, "RL", &RL_read_OK, offset);

				//if the read was good set the RL value into the terms struct
				if (RL_read_OK)
				{
					printf("Read in 1 variables in Terms line %d, RL=%g\n", line, *RL);

					termVal->RL = *RL;
				}

				// not a valid RL value, exit program
				else
				{
					printf("Read of line %d failed : %s\n", line, termsLine);
					if (*RL < 0)
					{
						printf("Not a valid RL value. Exiting...\n");
						exit(-1);
					}
				}
			}

			//else the digit was too big or too small compared to the largest node in the circuit
			else
			{
				printf("RL placement is not valid: %s for number of nodes: %d \n Exiting...\n", termsLine, *noOfNodes);
				exit(-1);
			}
		}
		//else the first element was not a digit
		else
		{
			printf("RL placement is not valid: %s\n Exiting...\n", termsLine);
			exit(-1);
		}
	}
}

void find_n_store_circuit(FILE *fileptr, char *circuitLine, int line, ImpMat *impArr)
{
	// reset line read to beginning of the file
	if (fseek(fileptr, 0, SEEK_SET) != 0)
	{
		char command[50];

		strcpy_s(command, sizeof command, "dir");  // DOS/Windows instruction to list out name and contents of current directory

		system(command);		// windows/dos system should print out contents of current directory where program is looking for file

		perror("Error: ");		// prints description of the error

		exit(-1);
	}


	while ((strstr(circuitLine, "</CIRCUIT>")) == NULL)
	{
		fgets(circuitLine, 150, fileptr);// resets file pointer to next line in file

		// if there is a blank line in the text file, skip and continue reading the file
		if (strcmp(circuitLine, "\n") == 0)
		{
			continue;
		}

		// if comment line, skip
		if (strstr(circuitLine, "#"))
		{
			continue;
		}

		//once CIRCUIT block beginning delimiter is reached, determine number of component values
		if (strstr(circuitLine, "<CIRCUIT>"))
		{
			store_circuit_block(fileptr, circuitLine, line, impArr); // reread the circuit block with array of proper size and store values 			
		}

		// else not CIRCUIT block exists
		if (feof(fileptr))
		{
			printf("No CIRCUIT block in file. Exiting...");

			exit(-1);
		}
	}
}

void store_circuit_block(FILE *fileptr, char *circuitLine, int line, ImpMat *impArr)
{
	//initialise local variables for the circuit read and storage
	int n1, n2;

	double compVal;

	char comp;

	int offset = 2;

	int gReadCheck = 0;

	int n1ReadCheck = 0;

	int	n2ReadCheck = 0;

	int rReadCheck = 0;

	while (1)
	{
		fgets(circuitLine, 150, fileptr);

		// if there is a blank line in the text file, skip and continue reading the file
		if (strcmp(circuitLine, "\n") == 0)
		{
			continue;
		}

		// if comment line, skip
		if (strstr(circuitLine, "#"))
		{
			continue;
		}

		// when the end delimiter is reached, leave circuit_read
		if (strstr(circuitLine, "</CIRCUIT>"))
		{
			break;
		}

		n1 = find_intVal(circuitLine, "n1", &n1ReadCheck);

		n2 = find_intVal(circuitLine, "n2", &n2ReadCheck);

		/*This puts the value of R into its correct place based on whether it is a circuit shunt or series resistance
		n1 and n2 will be reordered when stored so that n1 is always less than n2
		*/
		if (strstr(circuitLine, "R="))
		{
			compVal = find_compVal(circuitLine, "R=", &rReadCheck, offset);

			comp = 'R';
		}
		else if (strstr(circuitLine, "G="))
		{
			compVal = find_compVal(circuitLine, "G=", &gReadCheck, offset);

			comp = 'G';
		}
		else
		{
			printf("No component value in line %d. Exiting...");

			exit(-1);
		}
		if (rReadCheck && n1ReadCheck && n2ReadCheck)
		{
			store_R(impArr, &n1, &n2, &compVal, line);
			
			impArr = impArr + 1;
		}


		/*This puts the value of G into its correct place based on whether it is a circuit shunt or series resistance
		n1 and n2 will be reordered when stored so that n1 is always less than n2*/

		else if (gReadCheck && n1ReadCheck && n2ReadCheck)
		{
			store_G(impArr, &n1, &n2, &compVal, line);

			impArr = impArr + 1;
		}

		else
		{
			printf("Read of line %d failed\n", line);

			break;
		}

		line = line + 1;

	}
}

void store_R(ImpMat *impArr, int *n1, int *n2, double *compVal, int line)
{
	if ((*n1 == 0) || (*n2 == 0))
	{
		impArr->A = 1;

		impArr->B = 0;

		impArr->C = (1 / *compVal);// component was read as R series resistance but is in a shunt configuration

		impArr->D = 1;

		impArr->index = line;

		// reorders n1 and n2 to be sequential if out of order 
		if (*n1 == 0)
		{
			impArr->n1 = *n2;

			impArr->n2 = *n1;
		}
		else
		{
			impArr->n1 = *n1;

			impArr->n2 = *n2;
		}
	}
	else
	{
		impArr->A = 1;

		impArr->B = *compVal;

		impArr->C = 0;

		impArr->D = 1;

		impArr->index = line;

		// reorders n1 and n2 to be sequential
		if (*n1 > *n2)
		{
			impArr->n1 = *n2;

			impArr->n2 = *n1;
		}
		else
		{
			impArr->n1 = *n1;

			impArr->n2 = *n2;
		}
	}
}

void store_G(ImpMat *impArr, int *n1, int *n2, double *compVal, int line)
{
	// if the component is a shunt, a node will have a zero value
	if ((*n1 == 0) || (*n2 == 0))
	{
		impArr->A = 1;

		impArr->B = 0;

		impArr->C = *compVal;//component shunt values are in the C of the matrix

		impArr->D = 1;

		impArr->index = line;

		// reorders n1 and n2 to be sequential eg. ground (or zero) is always n2
		if (*n1 == 0)
		{
			impArr->n1 = *n2;

			impArr->n2 = *n1;
		}
		else
		{
			impArr->n1 = *n1;

			impArr->n2 = *n2;
		}
	}
	else
	{
		impArr->A = 1;

		impArr->B = (1 / *compVal);//component was read as G conductance but is in a series configuration

		impArr->C = 0;

		impArr->D = 1;

		impArr->index = line;

		// reorders n1 and n2 to be sequential
		if (*n1 > *n2)
		{
			impArr->n1 = *n2;

			impArr->n2 = *n1;
		}
		else
		{
			impArr->n1 = *n1;

			impArr->n2 = *n2;
		}
	}
}

void find_output(FILE *fileptr, char *file_line, int line, Outs *outArr, char *outFileName)
{
	// reset line read to beginning of the file
	if (fseek(fileptr, 0, SEEK_SET) != 0)
	{
		char command[50];
		strcpy_s(command, sizeof command, "dir");  // DOS/Windows instruction to list out name and contents of current directory
		system(command);		// windows/dos system should print out contents of current directory where program is looking for file
		perror("Error: ");		// prints description of the error
		exit(-1);
	}
	while ((strstr(file_line, "</OUTPUT>")) == NULL)
	{
		fgets(file_line, 150, fileptr);

		// if comment line, skip
		if (strstr(file_line, "#"))
		{
			continue;
		}

		//once the OUTPUT block beginning delimiter is reached, determine outputs needed
		if (strstr(file_line, "<OUTPUT>"))
		{
			read_output_block(fileptr, file_line, line, outArr, outFileName);
		}

		// when the end delimiter is reached, leave output_read
		if (strstr(file_line, "</OUTPUT>"))
		{

			break;
		}
	}
}

void read_output_block(FILE *fileptr, char *outputLine, int line, Outs *outArr, char *outFileName)
{
	int out_file_read_OK = 0;
	while ((strstr(outputLine, "</OUTPUT>")) == NULL)
	{
		fgets(outputLine, 150, fileptr);

		// if there is a blank line in the text file, skip and continue reading the file
		if (strcmp(outputLine, "\n") == 0)
		{
			continue;
		}

		// if comment line, skip
		if (strchr(outputLine, '#'))
		{
			continue;
		}

		//finds the designated output file name
		if (strstr(outputLine, "File_name"))
		{
			find_name(outputLine, "File_name", &out_file_read_OK, outFileName);
		}

		/*Checks for outputs and stores them in order in struct array outArr.
		After storage of output variable, array pointer is incremented to next element in struct array.
		Each output is tested whether dB is needed or not and each output's units are stored with respect to this value.*/
		else if (strstr(outputLine, "Vin"))
		{
			store_output(outArr, outputLine, "Vin", "Vin dB", "dBV", "V");

			outArr++;//increment output struct array to next element
		}
		else if (strstr(outputLine, "Vout"))
		{
			store_output(outArr, outputLine, "Vout", "Vout dB", "dBV", "V");

			outArr++;			
		}

		else if (strstr(outputLine, "Iin"))
		{
			store_output(outArr, outputLine, "Iin", "Iin dB", "dBI", "I");

			outArr++;
			
		}
		else if (strstr(outputLine, "Iout"))
		{
			store_output(outArr, outputLine, "Iout", "Iout dB", "dBI", "I");

			outArr++;
			
		}

		else if (strstr(outputLine, "Zin"))
		{
			store_output(outArr, outputLine, "Zin", "Zin dB", "dBOhm", "Ohm");

			outArr++;

		}
		else if (strstr(outputLine, "Zout"))
		{
			store_output(outArr, outputLine, "Zout", "Zout dB", "dBOhm", "Ohm");

			outArr++;
			/*strcpy_s(outArr->outputType, 20, "Zout");
			strcpy_s(outArr->units, 5, "Ohm");
			outArr++;*/

		}
		else if (strstr(outputLine, "Pin"))
		{
			store_output(outArr, outputLine, "Pin", "Pin dB", "dBW", "W");

			outArr++;
			
		}
		else if (strstr(outputLine, "Pout"))
		{
			store_output(outArr, outputLine, "Pout", "Pout dB", "dBW", "W");

			outArr++;
			
		}

		else if (strstr(outputLine, "Av"))
		{
			store_output(outArr, outputLine, "Av", "Av dB", "dB", "L");

			outArr++;

		}

		else if (strstr(outputLine, "Ai"))
		{
			store_output(outArr, outputLine, "Ai", "Ai dB", "dB", "L");

			outArr++;
		}

		else
		{
			break;
		}

	}
}

void store_output(Outs *outArr, char *outputLine, char *outName, char *dBName, char *dBType, char *outType)
{
	//if dB is in the Ouput string of the file, store correct output designator 
	if (strstr(outputLine, dBName))
	{
		strcpy_s(outArr->outputType, 20, outName);

		outArr->dB = 1; // if dB is needed, set dB to one

		strcpy_s(outArr->units, 5, dBType); // store units associated with the output
		
	}
	else
	{
		strcpy_s(outArr->outputType, 20, outName);

		strcpy_s(outArr->units, 5, outType);// store units associated with the output
		
	}
}

void calc_n_store_outputs(double **ABCD, Terms *termVal, Outs *outArr)
{
	/*Create variables using ABCD matrix and terminal values
	for readabity and clarity of calculations*/
	double A, B, C, D, RS, RL, VT, IN, YL;

	double Zin, Zout, Iin, Iout, Vin, Vout, Pin, Pout, Av, Ai;

	A = ABCD[0][0];

	B = ABCD[0][1];

	C = ABCD[1][0];

	D = ABCD[1][1];

	RS = termVal->RS;

	RL = termVal->RL;

	YL = 1 / (termVal->RL);

	if (termVal->Vt > 0)
	{
		VT = termVal->Vt;
	}
	else
	{
		IN = termVal->In;
		VT = IN * RS;
	}

	/*Calcualte output values*/
	Zin = (((A*RL) + B) / ((C*RL) + D));

	Zout = (((D*RS) + B) / ((C*RS) + A));

	Vin = (VT*Zin) / (Zin + RS);

	Iin = Vin / Zin;

	Vout = Vin / (A + (B * YL));

	Iout = Iin / (D + (C * RL));

	Pin = Vin * Iin;

	Pout = Vout * Iout;

	Av = Vout / Vin;

	Ai = Iout / Iin;


	/*Search through output array for output values
		Place results of calculations in outputValue of struct
		If dB is needed, calculate dB then place in struct*/
	for (int i = 0; i < 10; i++)
	{
		if (strstr(outArr[i].outputType, "Zin"))
		{
			outArr[i].outputValue = Zin;
		}
		else if (strstr(outArr[i].outputType, "Zout"))
		{
			outArr[i].outputValue = Zout;
		}
		else if (strstr(outArr[i].outputType, "Ai"))
		{

			if (outArr[i].dB == 1)
			{
				outArr[i].outputValue = 20 * log10(Ai);
			}
			else
			{
				outArr[i].outputValue = Ai;
			}
		}
		else if (strstr(outArr[i].outputType, "Av"))
		{

			if (outArr[i].dB == 1)
			{
				outArr[i].outputValue = 20 * log10(Av);
			}
			else
			{
				outArr[i].outputValue = Av;
			}
		}
		else if (strstr(outArr[i].outputType, "Vin"))
		{

			if (outArr[i].dB == 1)
			{
				outArr[i].outputValue = 20 * log10(Vin);
			}
			else
			{
				outArr[i].outputValue = Vin;
			}

		}
		else if (strstr(outArr[i].outputType, "Vout"))
		{

			if (outArr[i].dB == 1)
			{
				outArr[i].outputValue = 20 * log10(Vout);
			}
			else
			{
				outArr[i].outputValue = Vout;
			}
		}
		else if (strstr(outArr[i].outputType, "Iin"))
		{

			if (outArr[i].dB == 1)
			{
				outArr[i].outputValue = 20 * log10(Iin);
			}
			else
			{
				outArr[i].outputValue = Iin;
			}
		}
		else if (strstr(outArr[i].outputType, "Iout"))
		{

			if (outArr[i].dB == 1)
			{
				outArr[i].outputValue = 20 * log10(Iout);
			}
			else
			{
				outArr[i].outputValue = Iout;
			}
		}
		else if (strstr(outArr[i].outputType, "Pin"))
		{

			if (outArr[i].dB == 1)
			{
				outArr[i].outputValue = 10 * log10(Pin);
			}
			else
			{
				outArr[i].outputValue = Pin;
			}
		}
		else if (strstr(outArr[i].outputType, "Pout"))
		{

			if (outArr[i].dB == 1)
			{
				outArr[i].outputValue = 10 * log10(Pout);
			}
			else
			{
				outArr[i].outputValue = Pout;
			}
		}
		else
		{
			continue;
		}
	}
}

double find_compVal(char *file_line, char *component, int *ok_read, int offset)
{
	char *dblHolder; //dbl_holder variable that holds the result of the string comparison

	char prfxHolder = (char *)malloc(sizeof(char) * 2);

	double rtn = (double)(-1.234567);//initialise return value for the component

	int compVal, i_ok = 0;

	dblHolder = strstr(file_line, component);//determine if component to be found is in the file line and store in variable dbl_holder

	//if the component is found determine its value for placement in struct array
	if (dblHolder)
	{
		compVal = sscanf_s((dblHolder + offset), " %lf", &rtn); // sscanf_s returns the number of variables filled

		//if sscanf_s returns less than zero no variables were filled; if rtn remains initialsed value, bad read occured
		if ((compVal < 0) || (rtn < 0))
		{
			i_ok = 0;
		}
		else
		{
			check_prefix(dblHolder, &rtn);//check if component value has prefix 

			i_ok = 1;
		}

	}
	else
	{
		i_ok = 0;//component could not be found 
	}
	*ok_read = i_ok;//returns value for successful or unsucceessful read

	return(rtn);//returns component value
}

int find_intVal(char *file_line, char *node, int *ok)
{
	char *intholder;//intholder variable that holds the index of the strstr comparison

	int rtn = -1234567;//initialise return value for the node

	int intValue, i_ok, nodeFound = 0;

	intholder = strstr(file_line, node);

	//if the node is found determine its value for placement in struct array
	if (intholder)
	{
		intValue = sscanf_s((intholder + 3), " %d", &rtn);//sscanf_s returns the number of variables filled

		//if rtn is less than zero, unsuccessful read occured
		if ((intValue < 0) || (rtn < 0))
		{
			i_ok = 0;
		}
		else
		{
			i_ok = 1;
		}
	}
	//node read was unsuccessful
	else
	{
		i_ok = 0;
	}
	*ok = i_ok;

	return(rtn);
}

void find_name(char *file_line, char *nameToFind, int *ok_read, char *fileName)
{
	char *fileNameHolder, *fileNameHolder2; //fileNameHolder pointers that holds the result of the string comparison

	int fileFound, i_ok = 0;

	fileNameHolder = strstr(file_line, nameToFind);//returns a pointer to the first occurrence in haystack of any of the entire sequence of characters specified in needle

	//if the file name is found store it in the string fileName
	if (fileNameHolder)
	{
		fileNameHolder2 = strchr(fileNameHolder, '=');

		if (fileNameHolder2)
		{
			strcpy_s(fileName, 50, (fileNameHolder2 + 1)); // copies the file name found to the string fileName

			fileFound = strcmp(fileName, (fileNameHolder2 + 1));//strcmp returns zero for equality of two strings
		}

		// file read has been successful
		if (fileFound == 0)
		{
			i_ok = 1;
			/*replaces the new line of fileName with null termination
			 strcspn finds the index of the \n character in the string*/
			 //fileName[strcspn(fileName, "\n")] = 0;
			strcpy_s(fileName, 50, "Output_test.dat");

		}
		//file read is unsucessful
		else
		{
			i_ok = 0;
		}

	}
	else
	{
		i_ok = 0;//file read has been unsucessful 
	}

	*ok_read = i_ok;//returns value for successful or unsucceessful read

}

void check_prefix(char *compLine, double *compValue)
{
	if (strchr(compLine, 'p'))
	{
		*compValue = *compValue * (pow(10, 12));
	}
	else if (strchr(compLine, 'n'))
	{
		*compValue = *compValue * (pow(10, 9));
	}
	else if (strchr(compLine, 'u'))
	{
		*compValue = *compValue * (pow(10, 6));
	}
	else if (strchr(compLine, 'm'))
	{
		*compValue = *compValue * (pow(10, 3));
	}
	else if (strchr(compLine, 'k'))
	{
		*compValue = *compValue * (pow(10, -3));
	}
	else if (strchr(compLine, 'M'))
	{
		*compValue = *compValue * (pow(10, -6));

	}
	else if (strchr(compLine, 'G'))
	{
		*compValue = *compValue * (pow(10, -9));

	}
	else
	{
		//compValue remains unchanged
	}

}

int compare_nodes(const void *p, const void *q)
{
	ImpMat *p1 = (void *)p;//set pointer p1 to type struct ImpedanceMatrix  q
	ImpMat *p2 = (void *)q;//set pointer p2 to type struct ImpedanceMatrix  p

	// if p1 node1 is less than p2 node1, the value pointed at by p1 is first
	if ((p1->n1 - p2->n1) < 0)
	{
		return(-1);
	}

	// if p1 node1 is greater than p2 node1, the value pointed at by p2 is first
	else if ((p1->n1 - p2->n1) > 0)
	{
		return(1);
	}
	// else the node1 values are the same
	else
	{
		// if the p1 node2 value is less than p2 node2 value; ie p1 node2 is 0, p1 goes first
		if ((p1->n2 - p2->n2) < 0)
		{
			return(-1);
		}

		// if p2 node2 value is less than p1 node2; ie p2 node2 is 0 , p2 goes first 
		else if ((p1->n2 - p2->n2) > 0)
		{
			return(1);
		}
		// else both nodes are equal check index for necessary swap
		else
		{   //components are out of read in order, p2 goes first
			if ((p1->index - p2->index) > 0)
			{
				return (1);
			}

			// else no swap
			else
			{
				return (0);
			}

		}
	}



}

void print_struct_array(ImpMat *impArr, int len)
{
	int i;
	for (i = 0; i < len; i++)

		printf("Struct Array element %d\n[ n1=%d n2=%d B: %lf  C: %lf ]\n\n", i, impArr[i].n1, impArr[i].n2, impArr[i].B, impArr[i].C);

	puts("--");
}

void print_outputs(Outs *outArr, FILE *outputFilePtr)
{
	int i, j, k;
	for (i = 0; i < 10; i++)
	{
		if (i < 9)
		{
			//printf("%s, ", outArr[i].outputType);
			fprintf(outputFilePtr, "%s, ", outArr[i].outputType);
		}
		else
		{
			//printf("%s\n", outArr[i].outputType);
			fprintf(outputFilePtr, "%s\n", outArr[i].outputType);
		}
	}
	/*Print the second line of units for each output*/
	for (j = 0; j < 10; j++)
	{
		if (j < 9)
		{
			//printf("%s, ", outArr[j].units);
			fprintf(outputFilePtr, "%s, ", outArr[j].units);
		}
		else
		{
			//printf("%s\n", outArr[j].units);
			fprintf(outputFilePtr, "%s\n", outArr[j].units);
		}
	}
	for (k = 0; k < 10; k++)
	{
		if (k < 9)
		{
			//printf("%e, ", outArr[k].outputValue);
			fprintf(outputFilePtr, "  %1.4e,", outArr[k].outputValue);
		}
		else
		{
			//printf("%e\n", outArr[k].outputValue);
			fprintf(outputFilePtr, " %1.4e\n", outArr[k].outputValue);
		}
	}

}

void init_ABCD(double **ABCD, ImpMat *impArr)
{
	//sets values of the first element in the struct array to the ABCD matrix's A, B, C and D 
	ABCD[0][0] = impArr[0].A;
	ABCD[0][1] = impArr[0].B;
	ABCD[1][0] = impArr[0].C;
	ABCD[1][1] = impArr[0].D;
}

void calc_ABCD(double **ABCD, ImpMat *impArr, int circuit_size)
{
	//initialise A, B, C and D variables for readability
	double A = ABCD[0][0];

	double B = ABCD[0][1];

	double C = ABCD[1][0];

	double D = ABCD[1][1];

	for (int i = 0; i < (circuit_size - 1); i++)
	{
		
		ABCD[0][0] = (A*(impArr[i + 1].A)) + (B*(impArr[i + 1].C));

		ABCD[0][1] = (A*(impArr[i + 1].B)) + (B*(impArr[i + 1].D));

		ABCD[1][0] = (C*(impArr[i + 1].A)) + (D*(impArr[i + 1].C));

		ABCD[1][1] = (C*(impArr[i + 1].B)) + (D*(impArr[i + 1].D));

		//reset A,B,C and D to the new values in ABCDmtrx
		A = ABCD[0][0];

		B = ABCD[0][1];

		C = ABCD[1][0];

		D = ABCD[1][1];
		
		printf("[ %lf \t %lf ]\n[ %lf \t %lf ]\n", A, B, C, D);

		puts("--");
	}
}


