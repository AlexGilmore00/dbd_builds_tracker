#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MAX_LINE_LEN 256

// portable screen clear
#ifdef _WIN32
#define clrscr() system("cls")
#else
#define clrscr() printf("\e[1;1H\e[2J")
#endif

// for portable get terminal height
#ifdef _WIN32
#include<windows.h>
int getTermHeightWin32();
#define getTermHeight() getTermHeightWin32()
#else
#include<sys/ioctl.h>
#include<unistd.h>
int getTermHeightLinux();
#define getTermHeight() getTermHeightLinux()
#endif

/*
  clears the input buffer
*/
void clearInBuff()
{
    while(getchar() != '\n');
}

/*
  frees an array of of dynamically allocated character pointers
  of size 'size'
*/
void freeArrChar(char **array, int size)
{
    for (int i = 0; i < size; i ++)
    {
        free(array[i]);
    }
    return;
}

/*
  returns the integer of integer x to the power of integer y
  if y is negative, return max negative signed 32 bit int
*/
int power(int x, int y)
{
    if (y == 0)
        return 1;
    else if (y == 1)
        return x;
    else if (y < 0)
        return -__INT16_MAX__;
    
    int retVal = x;
    for (int i = 0; i < y - 1; i++)
    {
        retVal *= x;
    }

    return retVal;
}

/*
  stores each remaining line from the current point in db onwards into
  the array of strings lines. lines should have an array size large enough
  to hold the remaining line count
*/
void storeFile(FILE *db, char **lines)
{
    int currentLine = 0;
    char *line = malloc(MAX_LINE_LEN);
    while(!feof(db) && !ferror(db))
    {
        if (fgets(line, MAX_LINE_LEN, db) != NULL)
        {
            lines[currentLine] = line;
            currentLine++;
            line = malloc(MAX_LINE_LEN);
        }
    }
    free(line);
    rewind(db);
}

/*
  returns the number of lines in the passed file
*/
int getLineCount(FILE *file)
{
    int lineCount = 0;
    for (char c = getc(file); c != EOF; c = getc(file))
    {
        if (c == '\n')
            lineCount++;
    }
    rewind(file);

    return lineCount;
}

/*
  takes a pointer to a string containing a full line
  taken from a csv as well as nunmber to denote which
  collum index should be returned [0, 1, 2, ...] then
  returns a pointer to a string containing only that
  collum
*/
char * retrieveCollumn(char *line, int collum)
{
    char *retstr = malloc(MAX_LINE_LEN);
    char *movptr = &retstr[0];

    // move the line pointer along to the desired collumn
    for (int i = 0; i < collum; i++)
    {
        while (*line != ',' && *line != '\n' && *line != '\0')
        {
            line++;
        }
        // if pointer reaches end of line, exit with an error
        if (*line == '\n' || *line == '\0')
        {
            printf("input collum greater than the greatest index"
                "of collumns in the db was used");
            exit(-1);
        }
        line++;
    }

    // copy the desired collumn into the return string
    while (*line != ',' && *line != '\n' && *line != '\0')
    {
        *movptr = *line;
        movptr++;
        line++;
    }
    *movptr = '\0';

    return retstr;
}

/*
  gets all the information of given line 'line' of the passed file
  and stores it in output.
  the first line in the file is line 0
*/
void getFileLine(char *output, FILE *file, int line)
{
    int count = 0;
    while (!feof(file) && !ferror(file))
    {
        if (fgets(output, MAX_LINE_LEN, file) != NULL)
        {
            if (count == line)
            {
                rewind(file);
                return;
            }
            count++;
        }
    }

    printf("WARNING!! a line exceeding the lincount of the file"
        "was passed to utilities.c => getFileLine()\n");
    rewind(file);
    return;
}

/*
  formats the given csv build line 'line' into an easy to read
  layout for the user
*/
void rFormatLine(char *line)
{
    while (*line != '\0')
    {
        if (*line == ',')
            *line = '\n';

        if (*line == '-')
            *line = ' ';

        line++;
    } 
}

/*
  undoes the changes to a line done by rFormatLine so it can be
  writen to a csv again
*/
void rFormatUndo(char *line)
{
    while (*line != '\0')
    {
        if (*line == '\n')
            *line = ',';

        if (*line == ' ')
            *line = '-';

        line++;
    }

    // re-add the newline to the end, if it had one to begin with
    line--;
    if (*line == ',')
            *line = '\n';
}

/*
  takes an input of a desired output location and an array of 4 perks
  and formats the perks into a csv format before moving it to the
  desired output location
*/
void wFormatLine(char *output, char **perks)
{
    for (int i = 0; i < 4; i++)
    {
        char *pcpy = perks[i];
        while (*pcpy != '\n' && *pcpy != '\0')
        {
            if (*pcpy == ' ')
                *pcpy = '-';
            *output = *pcpy;
            output++;
            pcpy++;
        }
        // the newline will be skipped so we replace if with ',' here
        *output = ',';
        output++;
    }
    *output = '\0';
    // replace final ',' with \n
    output[strlen(output) - 1] = '\n';
}


/*
  takes two strings and compares them
  returns true of they are the same
  else returns false
  ' ' and '-' are considered the same
*/
bool cstrcmp(char *str1, char *str2)
{
    // cannot be the same if they are not the same length
    if (strlen(str1) != strlen(str2))
        return false;
    
    char *cpy1 = str1;
    char *cpy2 = str2;
    // compare each character, if any differences, return false
    for (int i = 0; i < strlen(str1); i++)
    {
        if ((*cpy1 != *cpy2)
            && !((*cpy1 == ' ' && *cpy2 == '-')
                || (*cpy1 == '-' && *cpy2 == ' ')))
        {
            return false;
        }
    }

    // if no discrepancies found, return true
    return true;
}

/*
  checks if the given line contains the given substring.
  returns 1 if trues
  returns 0 if false
  has some leniency to fit with differences in db format and user
  visible format
*/
int containsSubstring(char *line, char *subs)
{
    char *linecpy = line;
    char *subscpy = subs;
    while (*linecpy != '\n' && *linecpy != '\0')
    {
        if ((*linecpy == *subscpy)
            || (*linecpy == '-' && *subscpy == ' ')
            || (*linecpy == ',' && *subscpy == ' '))
        {
            subscpy++;
            // if the end of the input string is reached with all
            // characters matching some substring in the line,
            // add that line to matches
            if (*subscpy == '\n')
            {
                return 1;
            }
        }
        else
        {
            if (subscpy != subs)
            {
                subscpy = subs;
                // make sure to check this current character
                // in line again against the first input character
                // character again if input has to be reset
                linecpy--;
            }
        }
        linecpy++;
    }
    return 0;
}

/*
  takes an inputted string and returns the amount of lines it takes up
*/
int getNLCount(char *string)
{
    int len = strlen(string);
    int count = 0;
    for (int i = 0; i < len; i++)
    {
        if (string[i] == '\n')
        {
            count++;
        }
    }
    
    if (count == 0)
        return 1;
    else
    return count;
    
}

/*
  checks the next n characters (or up until a newline) at the top of the stdin
  buffer to see if they make a natural number.
  n is denoted by maxLen as that is the max character length of the number.
  if the number is not natural (<0 a/o decimal) or a non numerical character
  if found, a return value of -1 is given.
  otherwise, the number found is returned
*/
int getNumSTDIN(int maxLen)
{
    int numbers[maxLen];  // stores the nums from the individual characters 
    int finalNum = 0;  // will store the final return
    int counter = 0;
    char c = getchar();
    while (c != '\n' && counter < maxLen)
    {
        if (c >= '0' && c <= '9')
        {
            numbers[counter] = c - '0';
            counter++;
        }
        else
        {
            return -1;
        }
        c = getchar();
    }

    // make sure the input buffer is clear after function call
    if (c != '\n')
        clearInBuff();

    for (int i = 0; i < counter; i++)
    {
        finalNum += numbers[i] * power(10, counter - 1 - i);
    }

    if (finalNum != 0)
        return finalNum;
    else
        return -1;
}


/*
  takes a list of strings and displays them to the user
  and prompts them to select one to return. 
  returns: the index of the chosen item in the list
           if no option is selected, returns -1
*/
int chooseOptionMenu(char **list, int listCount, int initialOffset,  bool printSelection)
{
    // store which option the user is currently hovering over
    int curIndex = initialOffset;

    // keep track of this is the first loop in while loop for certian
    // ui features
    bool first = true;

    // print the contents to the user and prompt
    while (1)
    {
        char **lcpy = list;
        printf("please select one of these items to continue forward...\n"
            "[w(:x)] move cursor up (x lines)\n"
            "[s(:x)] move cursor down (x lines)\n"
            "[enter] select item\n"
            "[r] return, select nothing\n\n");
        
        // print chunk of items in the list so that they fit
        // neatly on the terminal display
        int loopCount;
        int offset = 8;  //number of newlines outside of items + 1 for second elipses
        if (first)
            offset += 2;  // +2 for first time to leave room for messages from prev menus
        if (curIndex != 0)
            offset += 1;  // +1 for first elipses showing more items before
        int height = getTermHeight();
        if (height == -1)
        {
            printf("unable to retrieve terminal information. printing all items.\n");
            loopCount = listCount - curIndex;
        }
        else
        {
            int itemSize = getNLCount(*lcpy) + 1;  /* assumes all items in list follow the same format
                                                      + 1 for empty line between each item
                                                   */ 
            int itemCount = (height - offset) / (itemSize);  // the number of items able to fit on the screen
            if (itemCount <= 0)
                itemCount = 1;  // make sure at least one will always be printed, even if it doesnt fit
            if (listCount - curIndex < itemCount)
            {
                // print all remaining items if they can fit
                loopCount = listCount - curIndex;
            }
            else
            {
                loopCount = itemCount;
            }
        }
        
        // display items
        if (curIndex != 0)
            printf("...\n");
        for (int i = curIndex; i < loopCount + curIndex; i++)
        {
            // item
            if (lcpy[i][strlen(lcpy[i]) - 1] != '\n')
                printf("%s\n", lcpy[i]);
            else
                printf("%s", lcpy[i]);

            // selector
            if (i == curIndex)
                printf("^^^^^^^^^^^^^^^\n");
            else
                printf("\n");
        }
        if (loopCount + curIndex < listCount)
            printf("...\n");

        // get user input
        char uInput = getchar();
        if (uInput != '\n' && uInput != 'w' && uInput != 's') // care about w and as as they can have additional args
            clearInBuff();

        int moveAmount;
        switch (uInput)
        {
        case 'w':
            // find if additional arg is given
            if (getchar() == ':')
            {
                moveAmount = getNumSTDIN(3);
                if (moveAmount == -1)
                    moveAmount = 1;
            }
            else
            {
                moveAmount = 1;
            }

            if (curIndex == 0)  // wrap around if at end of list
                curIndex = listCount - 1;
            else if (curIndex < moveAmount)  // snap to end if move amount is too high
                curIndex = 0;
            else
                curIndex -= moveAmount;
            break;
        case 's':
            // find if additional arg is given
            if (getchar() == ':')
            {
                moveAmount = getNumSTDIN(3);
                if (moveAmount == -1)
                    moveAmount = 1;
            }
            else
            {
                moveAmount = 1;
            }

            if (curIndex == listCount - 1)  // wrap around if at end of list
                curIndex = 0;
            else if (listCount - 1 - curIndex < moveAmount)  // snap to end if move amount is too high
                curIndex = listCount - 1;
            else
                curIndex +=moveAmount;
            break;
        case '\n':
            clrscr();
            if (printSelection)
                printf("you have selected option: %s\n", list[curIndex]);
            return curIndex;
            break;
        case 'r':
            clrscr();
            if (printSelection)
                printf("no selection made\n");
            return -1;
        default:
            printf("unknown command...\n");
            break;
        }
        clrscr();
        first = false;
    }
}

/*
  returns the number or rows on the terminal for windows machines
*/
int getTermHeightWin32()
{
    #ifndef _WIN32
    printf("WARNING!! tried calling win32 version of getTermHeight from a non"
        "-windows machine");
    return -1;
    #else

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return rows;

    #endif
}

/*
  returns the number or rows on the terminal for linux machines
*/
int getTermHeightLinux()
{   
    #ifndef __linux
    printf("WARNING!! tried calling linux version of getTermHeight from a non"
        "-linux machine");
    return -1;
    #else

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;

    #endif
}