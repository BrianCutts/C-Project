#include<stdio.h>

/*define a new type called impMat that holds the values of the ABCD "matrix" of each component*/
typedef struct ImpedanceMatrix {
	double A; //though this will always be one, it is used for readability and continuity
	double B; // the value of a series resistance gets placed here
	double C; // the value of a shunt conductance gets placed here
	double D; //though this will always be one, it is used for readability and continuity
	int n1;
	int n2;
	int index;//index of read in value to keep sorting psuedo-stable
} ImpMat;


/*define a new type called terms that stores the values of the terminal components*/
typedef struct Terminals {
	double Vt; //Thevenin voltage source
	double RS; //source resistance
	double In;//Norton Current
	double RL;//Load resistance
}Terms;


/*define a new type called Outputs that holds the outputs denoted in the file and also the final outputs after calculations
reuslts may be linear or in decibels*/
typedef struct Outputs {
	char outputType[20];//string to hold the output name: Vin, Vout etc
	int dB;
	char units[5];// string to hold units respective of the output i.e. for Vout: V, for Pout dB: dBW
	double outputValue;// holds the value of the output
}Outs;


/*Routine that checks for the CIRCUIT Delimiters calls read_circuit function
@param FILE *fileptr points to the file opened and being read
@param char *file_line points to the string that holds the line of the file to be read
@param int line integer that increments in the function and is used to find where line read error occured
@param int *crt_sz points to integer that stores the circuit size ie. number of components
@param int *noOfNodes points to the integer that holds the number of nodes used for error checking the load resistance in the terms block
*/
void find_circuit(FILE *fileptr, char *file_line, int line, int *crt_sz, int *noOfNodes);

/*Routine to read and record the size of the circuit to be analysed
@param FILE *fileptr points to the file opened and being read
@param char *circuitLine points to the string that holds the line of the file to be read
@param int line integer that increments in the function and is used to find where line read error occured
@param int *noOfNodes points to the integer that holds the number of nodes used for error checking the load resistance in the terms block
@return int returns the integer value of the circuit size/number of components in the circuit
*/
int read_circuit_block(FILE *fileptr, char *circuitLine, int line, int *noOfNodes);

/*Rountine that stores finds CIRCUIT demlimiter and calls store_circuit_block function
@param FILE *fileptr points to the file opened and being read
@param char *file_line points to the string that holds the line of the file to be read
@param int line integer that increments in the function and is used to find where line read error occured
@param impMat *impArr points the the struct array used to store component and node values
*/
void find_n_store_circuit(FILE *fileptr, char *file_line, int line, ImpMat *impArr);

/*Routine to store components from CIRCUIT block
@param FILE *fileptr points to the file opened and being read
@param char *circuitLine points to the string that holds the line of the file to be read
@param int line integer that increments in the function and is used to find where line read error occured
@param impMat *impArr points the the struct array used to store component and node values
*/
void store_circuit_block(FILE *fileptr, char *circuitLine, int line, ImpMat *impArr);

void store_R(ImpMat *impArr, int *n1, int *n2, double *compVal, int line);

void store_G(ImpMat *impArr, int *n1, int *n2, double *compVal, int line);

/*Rountine that finds and stores terminal values in struct terms
@param FILE *fileptr points to the file opened and being read
@param char *file_line points to the string that holds the line of the file to be read
@param int line integer that increments in the function and is used to find where line read error occured
@param terms *termVal points the the struct used to store terminal values
@param int *noOfNodes points to the integer that holds the number of nodes used for error checking the load resistance in the terms block
*/
void find_terms(FILE *fileptr, char *file_line, int line, Terms *termVal, int *noOfNodes);


/*Rountine that reads in terminal values TERMS block
@param FILE *fileptr points to the file opened and being read
@param char *termsLine points to the string that holds the line of the file to be read
@param int line integer that increments in the function and is used to find where line read error occured
@param terms *termVal points the the struct used to store terminal values
@param int *noOfNodes points to the integer that holds the number of nodes used for error checking the load resistance in the terms block
*/
void read_terms_block(FILE *fileptr, char *termsLine, int line, Terms *termVal, int *noOfNodes);


void check_n_store_terms(Terms *termVal, char *termsLine, double *VT, double *RS, double *IN, double *GS, int VT_read_OK, int IN_read_OK, int RS_read_OK, int GS_read_OK, int line);

void check_n_store_RL(Terms *termVal, char *termsLine, double *RL, int *noOfNodes, int *nodes, int *RL_read_OK, int offset, int line);


/*Rountine that finds and stores output values in struct outs
@param FILE *fileptr points to the file opened and being read
@param char *file_line points to the string that holds the line of the file to be read
@param int line integer that increments in the function and is used to find where line read error occured
@param outs *outs points the the struct used to store needed output values
@param char *outFileName points to the string that holds the file name for output of the calculated circuit*/
void find_output(FILE *fileptr, char *file_line, int line, Outs *outArr, char *outFileName);


/*Rountine that stores terminal values in struct terms
@param FILE *fileptr points to the file opened and being read
@param char *outputLine points to the string that holds the line of the file to be read
@param int line integer that increments in the function and is used to find where line read error occured
@param terms outs *outArr points the the struct used to store terminal values
@param char *outFileName points to the string that holds the file name for output of the calculated circuit*/
void read_output_block(FILE *fileptr, char *outputLine, int line, Outs *outArr, char *outFileName);

void store_output(Outs *outArr, char *outputLine, char *outName, char *dBName, char *dBType, char *outType);


void print_struct_array(ImpMat *array, int len);

/*Comparator function used for qsort()
@param const void *p and const void *p  are pointers used to compare and sort in the qsort function
@return int returns either 1 to sort pointer q before p, 0 for both equal and no sort, or -1 for sorting p before q*/
int compare_nodes(const void *p, const void *q);


/*Rountine to initialise the values in the ABCD matrix
@param double **ABCD points to the 2D array used to hold the values A,B,C and D
@param impMat *impArr points to the struct array where component node and resistance values are stored*/
void init_ABCD(double **ABCD, ImpMat *impArr);

/*Routine that initialises the 2x2 matrix ABCD to the A, B, C and D values of the first element of the struct array impArr
@param double **ABCD points to the 2x2 matrix
@param impMat *impArr points to the struct array with stored circuit values
@param int circuit_size integer value used to iterate through struct array*/
void calc_ABCD(double **ABCD, ImpMat *impArr, int circuit_size);

void calc_n_store_outputs(double **ABCD, Terms *termVal, Outs *outArr);

/*Routine to read and add string to variable fileName
@param char *file_line points to the string that holds the line of the file to be read
*/
void find_name(char *file_line, char *file, int *ok_read, char *fileName);

/*
Routine that finds the component value indicated by the variable passed called component.
@param char*file_line points to the string that holds the line of the file to be read
@param char*component points to the string that holds the component to find in the file
@param int *ok points to the integer denoting whether or not a successful read occured
@param int offset integer value that is used to find the numbers after the component is found
@return double returns the double of the component read in from file*/
double find_compVal(char *file_line, char *component, int *ok, int offset);


/*Routine that finds the  integer value indicated by the variable passed called node.
@param char *file_line points to the string that holds the line of the file to be read
@param char*component points to the string that holds the node to find in the file
@param int *ok points to the integer denoting whether or not a successful read occured
@return int returns the integer value of the node read in from the file*/
int find_intVal(char *file_line, char *node, int *ok);

void check_prefix(char *compLine, double *compValue);


void print_outputs(Outs *outArr, FILE *outputFilePtr);