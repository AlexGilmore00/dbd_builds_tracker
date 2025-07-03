/*
  bug needs fixing where sometimes deleting a build will end up causing the
  file to be rewritten wrong.

  maybe a chunk of malloced memory being reused after being freed and then a strings
  null terminator not being correctly replaced. i think this because, after this bug
  occurs on the survivor file, deleting a killer build will also cause inapopropriate
  survivor perks to show up in the killer builds file.
  the most likely place for this error to be occuring is when replaceing the header.
  i think this because the erronious rewrites have only been seen to occur after the header
  and before all other lines.

  SURV:
  perk1,perk2,perk3,perk4
  ,sh,blood-pact,champion-of-light
  ,ity
  boon:-circle-of-healing,appraisal,any-means-necessary,bond
  balanced-landing,smash-hit,vigil,shoulder-the-burden

  KILL:
  perk1,perk2,perk3,perk4,appraisal,any-means-necessary,bond

  NOTE: 
  boon:-circle-of-healing
  and
  perk1,perk2,perk3,perk4
  contain the same amount of characters, making sense of why...
  ,appraisal,any-means-necessary,bond
  follows both exactly.
  memory allocated for one line in surv build is being reused for rewriting header of
  killer builds.

  maybe fixed with this line update...
  was - memcpy(newLines[0], header, strlen(header));
  now - memcpy(newLines[0], header, strlen(header) + 1);
  this will copy the null terminator from header into newlines[0] now as well.
*/

#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include"write_to_file.h"
#include"read_file.h"
#include"utilities.h"

#ifdef _WIN32
#define pycall() system("pycall_win32.bat")
#else
#define pycall() system("sh pycall_linux.sh")
#endif

/*
  gives the user the option to call the perk_scraper.py file
  in order to replace and update the killer_perks.csv and 
  survivor_perks.csv files
*/
void updatePerkFiles()
{
    while (1)
    {
        printf("are you sure you'd like to update these files?\n"
            "[1] yes\n"
            "[2] no\n"
            "[3] display file contents\n");

        // get user input
        char input = getc(stdin);
        clearInBuff();
        clrscr();
        
        switch (input)
        {
        case '1':
            pycall();
            printf("press enter to continue...\n");
            while (getchar() != '\n');
            clrscr();
            return;
        case '2':
            return;
        case '3':
            printf("surviver perks:\n");
            printRawFile("./files/survivor_perks.csv");
            #ifndef _WIN32
            printf("\n\n\n\n\n");
            #endif
            printf("killer perks:\n");
            printRawFile("./files/killer_perks.csv");
            break;
        default:
            printf("unknown command...\n");
            break;
        }
    }
}

/*
    asks the user whether they want to proceed with the action for survivor
    files or killer files
    returns:  0 for survivor
              1 for killer
              2 for return
*/
int role_type()
{
    while (1)
    {
       printf("for which role?\n"
        "[1] survivor\n"
        "[2] killer\n"
        "[r] return to previous menu\n");

        // get user input
        char input = fgetc(stdin);
        clearInBuff();
        clrscr(); 

        switch (input)
        {
        case '1':
            return 0;
        case '2':
            return 1;
        case 'r':
            return 2;
        default:
            printf("unknown command...\n");
            break;
        }      
    }
}

/*
  prompts the user on options relating to
  reading from the existing builds
*/
void readOptions()
{
    while(1)
    {
        printf("choose reading option:\n"
        "[1] read whole build file\n"
        "[2] search build file\n"
        "[3] delete build entry\n"
        "[r] return to previous menu\n");

        // get user input
        char input = fgetc(stdin);
        clearInBuff();
        clrscr();

        int role;
        switch (input)
        {
        case '1':
            role = role_type();
            if (role == 0)
                printFile("./files/survivor_builds.csv");
            else if (role == 1)
                printFile("./files/killer_builds.csv");
            break;
        case '2':
            role = role_type();
            if (role == 0)
                searchFile("./files/survivor_builds.csv");
            else if (role == 1)
                searchFile("./files/killer_builds.csv");
            break;
        case '3':
            role = role_type();
            if (role == 0)
                deleteEntry("./files/survivor_builds.csv");
            else if (role == 1)
                deleteEntry("./files/killer_builds.csv");
            break;
        case 'r':
            return;
        default:
            printf("unknown command...\n");
            break;
        }
    }
}

/*
  prompts the user on whay method they would like to use
  in order to toggle the use of individual perks when generating
  a random build
*/
void perkToggleOptions()
{
    while (1)
    {
        printf("choose toggle option:\n"
            "[1] toggle individual perks\n"
            "[2] enable all perks for random use\n"
            "[3] disable all perks for random use\n"
            "[r] return to previous menu\n");
        
        // get user input
        char input = fgetc(stdin);
        clearInBuff();
        clrscr();
        
        int role;
        switch (input)
        {
        case '1':
            role = role_type();
            if (role == 0)
                togglePerks("./files/survivor_perks.csv");
            else if (role == 1)
                togglePerks("./files/killer_perks.csv");
            break;
        case '2':
            role = role_type();
            if (role == 0)
            {
                setAllPerks("./files/survivor_perks.csv", "true\n");
                printf("all perks enabled for use when generating a random survivor build\n\n");
            }
            else if (role == 1)
            {
                setAllPerks("./files/killer_perks.csv", "true\n");
                printf("all perks enabled for use when generating a random killer build\n\n");
            }
            break;
        case '3':
            role = role_type();
            if (role == 0)
            {
                setAllPerks("./files/survivor_perks.csv", "false\n");
                printf("all perks disabled for use when generating a random survivor build\n\n");
            }
            else if (role == 1)
            {
                setAllPerks("./files/killer_perks.csv", "false\n");
                printf("all perks disabled for use when generating a random killer build\n\n");
            }
            break;
        case 'r':
            return;
        default:
            printf("unknown command...\n");
            break;
        }
    }   
}

/*
  prompts the user on options relating to
  adding a new build to the db
*/
void writeOptions()
{
    while(1)
    {
        printf("choose writing option:\n"
        "[1] add build to build file\n"
        "[2] generate a random build\n"
        "[3] enable/disable perks for random builds\n"
        "[r] return to previous menu\n");

        // get user input
        char input = fgetc(stdin);
        clearInBuff();
        clrscr();

        int role;
        switch (input)
        {
        case '1':
            role = role_type();
            if (role == 0)
                writeToFile("./files/survivor_builds.csv", "./files/survivor_perks.csv");
            else if (role == 1)
                writeToFile("./files/killer_builds.csv", "./files/killer_perks.csv");
            break;
        case '2':
            role = role_type();
            if (role == 0)
                generateRandomBuild("./files/survivor_builds.csv", "./files/survivor_perks.csv");
            else if (role == 1)
                generateRandomBuild("./files/killer_builds.csv", "./files/killer_perks.csv");
            break;
        case '3':
            perkToggleOptions();
            break;
        case 'r':
            return;
        default:
            printf("unknown command...\n");
            break;
        }
    }
}

/*
  prompts the user on whether they want to add
  a build or read from existing builds
*/
int main(int argc, char **argv)
{
    clrscr();
    while(1)
    {
        printf("what would you like to do?\n"
        "[1] read from builds\n"
        "[2] add build\n"
        "[3] update perks files\n"
        "[e] exit\n");

        // get user input
        char input = fgetc(stdin);
        clearInBuff();
        clrscr();

        switch (input)
        {
        case '1':
            readOptions();
            break;
        case '2':
            writeOptions();
            break;
        case '3':
            updatePerkFiles();
            break;
        case 'e':
            return 0;
        default:
            printf("unknown command...\n");
            break;
        }
    }
}