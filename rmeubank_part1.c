#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define MAX_LINE 80 /* The maximum length command */

int fail = 0;

// Reiland Eubank
// CWID: 12183371

int main(void) {
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    while (should_run) {
        printf("Eubank %d>", (int)getpid());                          // prompt for command line
        fflush(stdout);

        // taking input from command line into char array
        char input[1023];
        fflush(stdout);
        fgets(input, sizeof(input), stdin);
        fflush(stdin);
        input[strcspn(input, "\n")] = 0;

        char *exitString = "exit";
        if (strcmp(input, "") == 0) {                           // checks for empty input, segfaults without this
            continue;
        }
        else if (strcmp(exitString,input) == 0) {               // checking for exit command
            should_run = 0;
            exit(0);
        }

        // parsing input into words for separate command and arguments
        char *delimiter = " ";
        char *subString = strtok(input, delimiter);
        int count = 0;
        while(subString != NULL) {
            args[count] = subString;
            subString = strtok(NULL, delimiter);
            count++;
        }
        
        // check for & as last argument
        int hasAmpersand = 0;
        char *ampersand = "&";
        if (strcmp(ampersand,args[count - 1]) == 0) {
            hasAmpersand = 1;               // final argument is &, parent process should wait
            args[count - 1] = NULL;
        }

        // ensure empty portion of array is filled with NULL values
        for(int i = count; i < MAX_LINE/2 + 1; i++) {
            args[i] = NULL;
        }

        // forks process and creates child, checking for fork failure
        int rc = fork();
        if (rc < 0) {
            // fork failed
            fprintf(stderr, "fork failed\n");
            exit(1);
        } 
        else if (rc == 0) {                         // child process, executes command
            fail = execvp(args[0], args);
        } 
        else {
            if (!hasAmpersand) {                    // parent process, waits on child if no &
                waitpid(rc, NULL, 0);
            }
            else {
                hasAmpersand = 0;
            }
            for(int i = 0; i < MAX_LINE/2 + 1; i++) {
                args[i] = NULL;
            }
        }

        if (fail == -1) {
            fail = 0;
            exit(0);
        }
        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) if command included &, parent will invoke wait()
        */
    }
return 0;
}