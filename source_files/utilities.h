#ifndef UTILS_H_
#define UTILS_H_

#include<stdbool.h>
#define MAX_LINE_LEN 256

// portable screen clear
#ifdef _WIN32
#define clrscr() system("cls")
#else
#define clrscr() printf("\e[1;1H\e[2J")
#endif

void clearInBuff();
void freeArrChar(char **, int);
int power(int, int);
void storeFile(FILE *, char **);
int getLineCount(FILE *);
char * retrieveCollumn(char *, int);
void getFileLine(char *, FILE *, int);
void rFormatLine(char *);
void rFormatUndo(char *);
void wFormatLine(char *, char **);
bool cstrcmp(char *, char *);
int containsSubstring(char *, char *);
int getNumSTDIN(int);
int chooseOptionMenu(char **, int, int, bool);

#endif