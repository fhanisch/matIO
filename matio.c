// cl /nologo /W3 matio.c

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { miINT8 = 1, miUINT8 = 2, miINT32 = 5, miUINT32 = 6, miDOUBLE = 9, miMATRIX = 14 } DataType;
typedef enum { mxDOUBLE_CLASS = 6 } ArrayType;

typedef struct
{
	DataType dataType;
	unsigned int dataSize;
	void *data;
} DataElement;

typedef struct
{
	DataElement flags;
	DataElement dimensions;
	DataElement name;
	DataElement pr;
} NumericArray;

typedef struct
{
	ArrayType class;
	unsigned int reserved;
} ArrayFlags;

typedef struct
{
	int m;
	int n;
} DimensionsArray;

const unsigned int tagLength = 8;

void checkArrayname(char *name)
{
	while (*name != 0)
	{
		if (*name == '(') *name = '_';
		else if (*name == ')') *name = 0;
		name++;
	}
}

int writeDoubleMatrix(char *name, double *data, int m, int n, FILE *matFile)
{
	DataElement matlabArray;
	NumericArray numArray;
	ArrayFlags arrayFlags;
	arrayFlags.class = mxDOUBLE_CLASS;
	arrayFlags.reserved = 0;
	DimensionsArray dimArray;
	dimArray.m = m;
	dimArray.n = n;
	char *arrayName;

	checkArrayname(name);

	unsigned int arrayNameLen = 8 + ((unsigned int)strlen(name) / 8)*8;
	arrayName = malloc(arrayNameLen);
	memset(arrayName, 0, arrayNameLen);
	strcpy(arrayName, name);

	numArray.flags.dataType = miUINT32;
	numArray.flags.dataSize = 8;
	numArray.flags.data = &arrayFlags;
	numArray.dimensions.dataType = miINT32;
	numArray.dimensions.dataSize = 8;
	numArray.dimensions.data = &dimArray;
	numArray.name.dataType = miINT8;
	numArray.name.dataSize = arrayNameLen;
	numArray.name.data = arrayName;
	numArray.pr.dataType = miDOUBLE;
	numArray.pr.dataSize = m*n*sizeof(double);
	numArray.pr.data = data;

	matlabArray.dataType = miMATRIX;
	matlabArray.dataSize = 4 * tagLength + numArray.flags.dataSize + numArray.dimensions.dataSize + numArray.name.dataSize + numArray.pr.dataSize;
	matlabArray.data = &numArray;

	fwrite(&matlabArray.dataType, 1, 4, matFile);
	fwrite(&matlabArray.dataSize, 1, 4, matFile);
	fwrite(&((NumericArray*)(matlabArray.data))->flags.dataType, 1, 4, matFile);
	fwrite(&((NumericArray*)(matlabArray.data))->flags.dataSize, 1, 4, matFile);
	fwrite(((NumericArray*)(matlabArray.data))->flags.data, 1, sizeof(ArrayFlags), matFile);
	fwrite(&((NumericArray*)(matlabArray.data))->dimensions.dataType, 1, 4, matFile);
	fwrite(&((NumericArray*)(matlabArray.data))->dimensions.dataSize, 1, 4, matFile);
	fwrite(((NumericArray*)(matlabArray.data))->dimensions.data, 1, sizeof(DimensionsArray), matFile);
	fwrite(&((NumericArray*)(matlabArray.data))->name.dataType, 1, 4, matFile);
	fwrite(&((NumericArray*)(matlabArray.data))->name.dataSize, 1, 4, matFile);
	fwrite(((NumericArray*)(matlabArray.data))->name.data, 1, arrayNameLen, matFile);
	fwrite(&((NumericArray*)(matlabArray.data))->pr.dataType, 1, 4, matFile);
	fwrite(&((NumericArray*)(matlabArray.data))->pr.dataSize, 1, 4, matFile);
	fwrite(((NumericArray*)(matlabArray.data))->pr.data, 1, ((NumericArray*)(matlabArray.data))->pr.dataSize, matFile);

	free(arrayName);
	return 0;
}

int writeHeader(char *msg, FILE *matFile)
{
	char header[128];
	time_t curtime;

	time(&curtime);
	memset(header, 0, 128);
	sprintf(header, "%s on %s", msg, ctime(&curtime));
	header[125] = 1;
	header[126] = 'I';
	header[127] = 'M';
	fwrite(header, 1, sizeof(header), matFile);

	return 0;
}

#ifndef LIB
int main(int argc, char *argv[])
{
	const char filename[] = "mymat.mat";
	double values[] = { 1.0, 4.0, 7.0, 2.0, 5.0, 8.0, 3.0, 6.0, 9.2 };

	printf("Create mat-File\n\n");

	FILE *myMatFile = fopen(filename, "wb");
	writeHeader("Created by WebFMU", myMatFile);
	writeDoubleMatrix("vehicleSpeed", values, 1, 9, myMatFile);
	writeDoubleMatrix("yData", values, 9, 1, myMatFile);
	writeDoubleMatrix("zData", values, 3, 3, myMatFile);
	fclose(myMatFile);

	return 0;
}
#endif