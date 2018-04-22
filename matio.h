#ifndef MATIO_H
#define MATIO_H

int writeHeader(char *msg);
int writeDoubleMatrix(char *name, double *data, int m, int n);

#endif // MATIO_H