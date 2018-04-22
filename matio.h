#ifndef MATIO_H
#define MATIO_H

int writeHeader(char *msg, FILE *matFile);
int writeDoubleMatrix(char *name, double *data, int m, int n, FILE *matFile);

#endif // MATIO_H