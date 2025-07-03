#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"write_to_file.h"
#include"utilities.h"

/*
  display a list of all build entries to the user and prompt
  them to pick one to remove and, if one is chosen, remove it 
  from the inputted file
*/
void deleteEntry(char *fileName)
{
    // open file
    FILE *db;
    db = fopen(fileName, "r");
    if (db == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to read_file.c => deleteEntry()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    // store file contents into an array of pointers to strings containing each line
    int lineCount = getLineCount(db);
    char header[MAX_LINE_LEN];
    fgets(header, MAX_LINE_LEN, db);  // remove header
    lineCount--;
    char *lines[lineCount];
    storeFile(db, lines);
    fclose(db);

    if (lineCount <= 0)
    {
        printf("no builds to delete\n"
            "press enter to continue...\n");
        while (getchar() != '\n');
        clrscr();
        return;
    }

    // format all lines for output to the user
    for (int i = 0; i < lineCount; i++)
    {
        rFormatLine(lines[i]);
    }

    // display all lines to the user and ask them to choose one to remove
    int iChoice = chooseOptionMenu(lines, lineCount, 0, true);
    if (iChoice == -1)
    {
        clrscr();
        printf("no build removed...\n");
        return;
    }

    // allocate memory for the new data
    // lineCount is unchanged as its +1 for inclusion of header
    // again but then -1 for removed build
    char *newLines[lineCount];
    for (int i = 0; i < lineCount; i++)
    {
        newLines[i] = malloc(MAX_LINE_LEN);
    }

    // copy the new file data into newlines
    memcpy(newLines[0], header, strlen(header) + 1);
    int pos = 1;
    for (int i = 0; i < lineCount; i++)
    {
        if (i != iChoice)
        {
            memcpy(newLines[pos], lines[i], strlen(lines[i]) + 1);  // + 1 to include null term
            pos++;
        }
    }

    // format newlines for writing
    for (int i = 0; i < lineCount; i++)
    {
        rFormatUndo(newLines[i]);
    }

    // write the new data to the file
    pureOverwrite(fileName, newLines, lineCount);
    printf("succesfully removed build:\n%s\n", lines[iChoice]);

    // free allocated memory
    freeArrChar(lines, lineCount);
    freeArrChar(newLines, lineCount);
    return;
}
/*
  searches the given builds file at fileName for lines containing
  a substring inputted by the user then displays all builds that
  contain a match
*/
void searchFile(char *fileName)
{
    // open file
    FILE *db;
    db = fopen(fileName, "r");
    if (db == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to read_file.c => searchFile()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    int lineCount = getLineCount(db);
    // store file contents into an array of pointers to strings containing each line
    char header[MAX_LINE_LEN];
    fgets(header, MAX_LINE_LEN, db);  // remove header
    lineCount--;
    char *lines[lineCount];
    storeFile(db, lines);

    // get user input for desired substring
    printf("enter the substring you want to search for:\n");
    char input[MAX_LINE_LEN];
    fgets(input, MAX_LINE_LEN, stdin);
    if (strlen(input) == MAX_LINE_LEN - 1 && input[MAX_LINE_LEN - 1] == '\n')
        clearInBuff();  // only clear input buffer if necessary
    clrscr();

    // store any matches found seperately
    char *matches[lineCount];
    int matchCount = 0;

    // look though each line to see if any contain the requested substring
    for (int i = 0; i < lineCount; i++)
    {
        if (containsSubstring(lines[i], input) == 1)
        {
            matches[matchCount] = lines[i];
            matchCount++;
        }
    }

    // output matches to the user
    input[strlen(input) - 1] = '\0';
    if (matchCount > 0)
    {
        printf("ALL ENTRIES CONTAINING \'%s\':\n", input);
        for (int i = 0; i < matchCount; i++)
        {
            rFormatLine(matches[i]);
            printf("%s\n", matches[i]);
        }
    }
    else
    {
        printf("no matches found containing \'%s\'\n", input);
    }
    printf("press enter to continue...");
    while (getchar() != '\n');
    clrscr();

    // free allocated memory
    freeArrChar(lines, lineCount);
    fclose(db);
    return;
}

/*
  outputs the formatted contents of the csv file found at fileName. 
  the cvs file should be either surviver_builds or killer_builds or
  other similar such files and so should have 4 collumns
*/
void printFile(char *fileName)
{
    // open file
    FILE *db;
    db = fopen(fileName, "r");
    if (db == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to read_file.c => printFile()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    // remove header
    char line[MAX_LINE_LEN];
    fgets(line, MAX_LINE_LEN, db);

    bool containsBuild = false;
    while(!feof(db) && !ferror(db))
    {
        char line[MAX_LINE_LEN];
        if (fgets(line, MAX_LINE_LEN, db) != NULL)
        {
            containsBuild = true;
            rFormatLine(line);
            printf("%s\n", line);
        }
    }

    if (!containsBuild)
        printf("no builds in this file\n");
    printf("press enter to continue...");
    while (getchar() != '\n');
    clrscr();

    fclose(db);
    return;
}

/*
  outputs the exact contents of the inputted file
*/
void printRawFile(char *fileName)
{
    // open file
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to read_file.c => printRawFile()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    // output file
    char line[MAX_LINE_LEN];
    while (!feof(file) && !ferror(file))
    {
        if (fgets(line, MAX_LINE_LEN, file) != NULL)
        {
            printf("%s", line);
        }
    }
    printf("press enter to continue...");
    while (getchar() != '\n');
    clrscr();

    fclose(file);
    return;
}