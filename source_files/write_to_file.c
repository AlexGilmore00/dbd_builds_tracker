#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"utilities.h"

/*
  takes an input and compares it to all the perks in allPerks to try and
  find any similar. it does this by...
  seeing if any perks contains the substring of input
  ...
  if a similar perk is found and accepted. the string stored at input is
  changed to the newly accepted string and this func returns true
  otherwise, if no similar perk is found or none are accepted, return false
*/
bool findSimilars(char *input, char **allPerks, int perkCount)
{
    // store any similar perks in an array, formatted ready for output
    char *similars[perkCount];
    int simCount = 0;
    for (int i = 0; i < perkCount; i++)
    {
        // first, see if the perk contains the substring input
        if (containsSubstring(allPerks[i], input) == 1)
        {
            similars[simCount] = malloc(MAX_LINE_LEN);
            memcpy(similars[simCount], allPerks[i], strlen(allPerks[i]) + 1);
            rFormatLine(similars[simCount]);
            simCount++;
        }
    }

    // have the user choose which similar perk to select
    if (simCount > 0)
    {
        int iChoice = chooseOptionMenu(similars, simCount, 0, true);
        if (iChoice >= 0)
        {
            // copy choice into the input
            rFormatUndo(similars[iChoice]);
            memcpy(input, similars[iChoice], strlen(similars[iChoice]) + 1);
            // terminate string at the end of the copied bytes
            input[strlen(similars[iChoice])] = '\n';
            input[strlen(similars[iChoice]) + 1] = '\0';

            // free memory
            freeArrChar(similars, simCount);
            return true;
        }
        else 
        {
            goto returnFalse;
        }
    }
    else
    {
        printf("no similar perks found...\n\n");
        goto returnFalse;
    }

    returnFalse:
    // free memory
    freeArrChar(similars, simCount);
    return false;
}

/*
  validates that perk is contained within the list of all perks provided
  returns true if it is valid
  else returns false
  note: perk will end in /n whereas the perks contained within allPerks
        will not
*/
bool validate(char *perk, char **allPerks, int perkCount)
{
    for (int i = 0; i < perkCount; i++)
    {
        char *pcpy = perk;
        char *compperk = *allPerks;
        int perkLen = strlen(pcpy);

        // of strings dont contain the same number of characters
        // they cannot be the same
        // - 1 makes up for perk having \n and compperk not
        if (perkLen - 1 != strlen(compperk))
        {
            allPerks++;
            continue;
        }

        // compare all characters
        for (int j = 0; j < perkLen; j++)
        {
            // if characters dont match, move onto the next perk
            if ((*pcpy != *compperk)
                && !(*pcpy == ' ' && *compperk == '-')
                && !(*pcpy == '\n' && *compperk == '\0'))
            {
                pcpy = perk;
                break;
            }
            // if gotten to the end of both words with no discrepancies
            // return true as match was found
            if (*pcpy == '\n' && *compperk == '\0')
            {
                return true;
            }
            pcpy++;
            compperk++;
        }

        // reset perk pointer and move onto next perk
        pcpy = perk;
        allPerks++;
    }

    // if no matches found, return false
    return false;
}

/*
  asks the user to input 4 perk names to write to the builds file
  storing them in wline.
  validating them to make sure they exist againt the perkListFile
  returns false if user decides to cancel the making of a new entrty
*/
bool getUserInput(char *wline, char *perkListFile)
{
    // allocate memory for user inputted perks
    char *inPerks[4];
    for (int i = 0; i < 4; i++)
    {
        inPerks[i] = malloc(MAX_LINE_LEN);
    }
    
    // open validation list
    FILE *pl;
    pl = fopen(perkListFile, "r");
    if (pl == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to write_to_file.c => getUserInput()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    // create array of all perk names
    int perkCount = getLineCount(pl) - 1;
    char *allPerks[perkCount];
    char header[MAX_LINE_LEN];  //remove header
    fgets(header, MAX_LINE_LEN, pl);
    char line[MAX_LINE_LEN];
    for (int i = 0; i < perkCount; i++)
    {
        fgets(line, MAX_LINE_LEN, pl);
        // retrieve only the collum containing the perk name
        allPerks[i] = retrieveCollumn(line, 0);
    }
    fclose(pl);

    // ask user perk for inputs
    for (int i = 0; i < 4; i++)
    {
        bool valid = false;
        while(!valid)
        {
            // get input
            printf("enter the name for the perk %d slot\n"
                "enter [r] to return to the previous menu\n"
                "enter \':list\' to choose from a list of all perks\n"
                "please use lowercase\n", i + 1);
            fgets(inPerks[i], MAX_LINE_LEN, stdin);
            clrscr();

            // check for return
            if (*inPerks[i] == 'r' && strlen(inPerks[i]) <=2)
                return false;

            // check for :list specification
            if (strcmp(inPerks[i], ":list\n") == 0)
            {
                // format perks for display
                for (int j = 0; j < perkCount; j++)
                    rFormatLine(allPerks[j]);
                int selection = chooseOptionMenu(allPerks, perkCount, 0, true);
                for (int j = 0; j < perkCount; j++)
                    rFormatUndo(allPerks[j]);
                memcpy(inPerks[i], allPerks[selection], strlen(allPerks[selection]) + 1);
                // add in expected newline
                inPerks[i][strlen(inPerks[i]) + 1] = '\0';
                inPerks[i][strlen(inPerks[i])] = '\n';
            }

            // validate perk exists
            valid = validate(inPerks[i], allPerks, perkCount);
            if (!valid)
            {
                // remove newline for printing
                inPerks[i][strlen(inPerks[i]) - 1] = '\0';
                printf("perk \'%s\' not recognised...\n", inPerks[i]);
                inPerks[i][strlen(inPerks[i])] = '\n';
                // search for valid perks similar to the input
                printf("looking for similar perks...\n");
                valid = findSimilars(inPerks[i], allPerks, perkCount);
            }
            // make sure perk was not already inputted
            bool hasDupe = false;
            for (int j = 0; j < 4; j++)
            {
                if (i != j)
                    hasDupe = cstrcmp(inPerks[i], inPerks[j]);

                if (hasDupe)
                {
                    valid = false;
                    // remove newline for printing
                    inPerks[i][strlen(inPerks[i]) - 1] = '\0';
                    printf("perk \'%s\' already included, multiple"
                        " of the same perk cannot be included in a build\n\n", inPerks[i]);
                    break;
                }
            }
        }
        printf("added perk %s\n", inPerks[i]);
    }

    // free list of all perks
    freeArrChar(allPerks, perkCount);

    // make sure all inputted perks combined does not
    // exceed max line length limit
    int totLen = 0;
    for (int i = 0; i < 4; i++)
    {
        totLen += strlen(inPerks[i]);
    }
    if (totLen > MAX_LINE_LEN)
    {
        printf("WARNING!! combination of inputted perks"
            "exceeds max line length limit for the csv\n"
            "returning to previous menu");
        return false;
    }

    // format perks into csv line
    wFormatLine(wline, inPerks);

    // free list of inputted perks
    freeArrChar(inPerks, 4);
    return true;
}

/*
  generates a random build for the user based off the perks in the
  given allPerksFile filepath and then asks if the user would like to
  store it in the given writeFile filepath
*/
void generateRandomBuild(char *writeFile, char *perkListFile)
{
    // open perks file
    FILE *pl;
    pl = fopen(perkListFile, "r");
    if (pl == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to write_to_file.c => generateRandomBuild()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    // remove header and get linecount
    int maxPerkCount = getLineCount(pl);
    char header[MAX_LINE_LEN];
    fgets(header, MAX_LINE_LEN, pl);
    maxPerkCount--;

    // get only the perks marked as 'true' in the 'random-enabled' section
    int activePerkCount = 0;
    int activePerks[maxPerkCount]; // stores the line numbers of all the active perks. -1 is used to denote end
    int counter = 1;  // start as 1 as, when file is rewound, header will be re-added so first perk is line 1 not 0 
    while(!feof(pl) && !ferror(pl))
    {
        char line[MAX_LINE_LEN];
        if (fgets(line, MAX_LINE_LEN, pl) != NULL)
        {
            char *active = retrieveCollumn(line, 2);
        if (strcmp(active, "true") == 0)
        {
            activePerks[activePerkCount] = counter;
            activePerkCount++;
        }
        counter++;
        free(active);
        }
    }
    activePerks[activePerkCount] = -1;
    rewind(pl);

    if (activePerkCount < 4)
    {
        printf("not enough perks enabled to make a random build, you must have at least "
            "4 perks enabled\npress enter to continue...\n");
        while (getchar() != '\n');
        clrscr();
        return;
    }

    // store random perks
    char *perks[4];

    // initialize randomizer
    srand((unsigned int)time(NULL));

    // get the random perks
    int alreadyChosen[4];
    int lineNum = 0;
    for (int i = 0; i < 4; i++)
    {
        bool valid = false;
        int failCount = 0;
        while (!valid)
        {
            
            // if rolled the same already chosen number 3 times in a row
            if (failCount >= 3)
            {
                lineNum += 1;
                lineNum %= activePerkCount;
            }
            else  // else roll new random num
            {
                lineNum = rand() % activePerkCount;
            }

            // make sure its not already been used
            for (int j = 0; j < i; j++)
            {
                if (alreadyChosen[j] == lineNum)
                {
                    valid = false;
                    failCount++;
                    break;
                }
                else
                {
                    valid = true;
                }
            }

            // first perk will always be valid
            if (i == 0)
                valid = true;
        }

        // convert the random lineNum to the actual line num and add the perk to the perks list
        alreadyChosen[i] = lineNum;
        char tempLine[MAX_LINE_LEN];
        getFileLine(tempLine, pl, activePerks[lineNum]);
        perks[i] = retrieveCollumn(tempLine, 0);
    }

    // format the perks into a csv line
    char *perksCSV = malloc(MAX_LINE_LEN);
    wFormatLine(perksCSV, perks);

    // display the build to the user and ask if they awnt to keep it
    while(1)
    {
        rFormatLine(perksCSV);
        printf("the build generated was:\n%s\n", perksCSV);
        rFormatUndo(perksCSV);
        printf("would you like to add this to your builds list?\n"
            "[1] yes\n"
            "[2] no\n");

        // get user input
        char input = getc(stdin);
        clearInBuff();
        clrscr();

        switch (input)
        {
        case '1':
            void pureAppend(char *, char *);
            pureAppend(writeFile, perksCSV);
            rFormatLine(perksCSV);
            printf("succesfully added build:\n%s\n", perksCSV);
            printf("press enter to continue...\n");
            while (getchar() != '\n');
            clrscr();
            goto returnBlock;
        case '2':
            goto returnBlock;
        default:
            printf("unknown command...\n");
            break;
        }
    }

    returnBlock:
    // free allocated memory
    freeArrChar(perks, 4);
    free(perksCSV);
    fclose(pl);
    return;
}

/*
  gives the user the option to enable/disable perks for use when generating
  a random build
*/
void togglePerks(char *perksFile)
{
    // keep track of the current line of the file you were looking at
    // for better user experience
    int currentLine = 0;
    while (1)
    {
        // open and store file
        FILE *pl;
        pl = fopen(perksFile, "r");
        if (pl == NULL)
        {
            printf("WARNING!! invalid file name passed "
                "to write_to_file.c => togglePerks()\n"
                "make sure you are running the executable file "
                "from the same directory it is located in\n");
            exit(-1);
        }
        int lineCount = getLineCount(pl);
        char *allPerks[lineCount];
        char *allPerksPrint[lineCount];  // a seperate storage of all perks formatted nicer for printing
        char header[MAX_LINE_LEN];
        fgets(header, MAX_LINE_LEN, pl);
        lineCount--;
        storeFile(pl, allPerks);
        fgets(header, MAX_LINE_LEN, pl);
        fclose(pl);

        // format perks for printing
        for (int i = 0; i < lineCount; i ++)
        {
            // just fucking do all this again like wtf even is this shit-----------------------
            // ok is redeemable now, doesnt crash but has wrong output, can probably be solved with debug
            char *name = retrieveCollumn(allPerks[i], 0);
            rFormatLine(name);
            char *enabled = retrieveCollumn(allPerks[i], 2);
            char *formattedLine = malloc(MAX_LINE_LEN);
            memcpy(formattedLine, name, strlen(name) + 1);  // + 1 to copy null terminator as well 
            strcat(formattedLine, " => enabled: \0");
            strcat(formattedLine, enabled);
            allPerksPrint[i] = formattedLine;
            free(name);
            free(enabled);
        }

        // let user choose which to toggle
        currentLine = chooseOptionMenu(allPerksPrint, lineCount, currentLine, false);
        if (currentLine == -1)
            return;

        // update the changed line
        char *changedLine = allPerks[currentLine];
        // move to 3rd collum
        for (int i = 0; i < 2; i++)
        {
            while (*changedLine != ',')
            {
                changedLine++;
            }
            changedLine++;
        }
        // update value
        if (*changedLine == 't')
            memcpy(changedLine, "false\n\0", 8);
        else
            memcpy(changedLine, "true\n\0", 7);

        // rewrite updated data to file
        FILE *plNew;
        plNew = fopen(perksFile, "w");
        if (plNew == NULL)
        {
            printf("WARNING!! invalid file name passed "
                "to write_to_file.c => togglePerks()\n"
                "make sure you are running the executable file "
                "from the same directory it is located in\n");
            exit(-1);
        }
        fprintf(plNew, "%s", header);
        for (int i = 0; i < lineCount; i++)
        {
            fprintf(plNew, "%s", allPerks[i]);
            free(allPerks[i]);
            free(allPerksPrint[i]);
        }
        fclose(plNew);
    }
}

/*
  sets the 'random enabled' field of all the perks in the inputted perksFile
  to the given setting og "true" or "false"
*/
void setAllPerks(char *perksFile, char *setting)
{
    if (!(strcmp(setting, "true\n") == 0 || strcmp(setting, "false\n") == 0))
    {
        printf("an invalid settings option was given to write_to_file.c"
        " => setAllPerks(). ending subroutine...");
        return;
    }

    // open and store file
    FILE *pl;
    pl = fopen(perksFile, "r");
    if (pl == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to write_to_file.c => setAllPerks()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }
    int lineCount = getLineCount(pl);
    char *allPerks[lineCount];
    storeFile(pl, allPerks);
    fclose(pl);

    // format allPerks to have desired setting for 'random_enabled'
    for (int i = 1; i < lineCount; i++)  // i = 1 to skip header
    {
        // find the closest seperating comma from the right to find the
        // start of the final collum
        int rPos = strlen(allPerks[i]);
        while (allPerks[i][rPos] != ',')
        {
            rPos--;
        }
        rPos++;
        allPerks[i][rPos] = '\0';
        // replace field 
        strcat(allPerks[i], setting);
    }

    // overwrite old file data with new
    void pureOverwrite(char *, char **, int);
    pureOverwrite(perksFile, allPerks, lineCount);

    // free memory
    freeArrChar(allPerks, lineCount);
    return;
}

/*
    takes arg writeFile which is the path the the respective builds csv
    and arg perkListFile which is the file that contains the names of all
    the valid inPerks
    takes validated input from the user for a build then adds it to writeFile
*/
void writeToFile(char *writeFile, char *perkListFile)
{
    char *wline = malloc(MAX_LINE_LEN);

    // get user input and return if user decides to terminate
    if (getUserInput(wline, perkListFile) == false)
        return;

    // open builds file
    FILE *buildsf;
    buildsf = fopen(writeFile, "a");
    if (buildsf == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to write_to_file.c => writeToFile()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    // write to file
    fprintf(buildsf, "%s", wline);

    // print confirmation
    rFormatLine(wline);
    printf("successfully added build:\n%s\n", wline);

    free(wline);
    fclose(buildsf);
    return;
}

/*
  takes a filepath and a list of data as well as the number of elements
  in said list and and overrites all the old data with the new data
*/
void pureOverwrite(char *writeFile, char **data, int dataLen)
{
    // open file
    FILE *fl;
    fl = fopen(writeFile, "w");
    if (fl == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to write_to_file.c => pureOverwrite()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    // write to file
    for (int i = 0; i < dataLen; i++)
    {
        fprintf(fl, "%s", data[i]);
    }

    fclose(fl);
    return;
}

/*
  takes a filepath and appends the given line of data to it
*/
void pureAppend(char *writeFile, char *data)
{
    // open file
    FILE *fl;
    fl = fopen(writeFile, "a");
    if (fl == NULL)
    {
        printf("WARNING!! invalid file name passed "
            "to write_to_file.c => pureAppend()\n"
            "make sure you are running the executable file "
            "from the same directory it is located in\n");
        exit(-1);
    }

    fprintf(fl, "%s", data);

    fclose(fl);
    return;
}