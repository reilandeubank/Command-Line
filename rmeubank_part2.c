#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define MAX_LINE 80 /* The maximum length command */
#define MAX_HISTORY 10 /* The maximum number of commands in history */

// Reiland Eubank
// CWID: 12183371

char history[MAX_HISTORY][MAX_LINE];
int history_count = 0;
int fail = 0;

void add_to_history(char *input) {
    // add command to history
    strcpy(history[history_count % MAX_HISTORY], input);
    history_count++;
}

void remove_from_history() {
    history_count--;
    history[history_count % MAX_HISTORY][0] = '\0';
}

int main(void) {
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    while (should_run) {
        printf("Eubank %d>", (int)getpid()); 
        fflush(stdout);

        // taking input from command line into char array
        char input[1023];
        char inputCopy[1023];
        fflush(stdout);
        fgets(input, sizeof(input), stdin);
        fflush(stdin);
        input[strcspn(input, "\n")] = 0;
        strcpy(inputCopy, input);

        char *exitString = "exit";
        if (strcmp(input, "") == 0) {                           // checks for empty input, segfaults without this
            continue;
        }
        else if (strcmp(exitString,input) == 0) {               // checking for exit command
            should_run = 0;
            exit(0);
        }

        // check for !! or !N command
        if (input[0] == '!') {
            if (input[1] == '!') {
                // retrieve most recent command
                if (history_count == 0) {
                    printf("No commands in history.\n");
                    continue;
                }
                strcpy(input, history[(history_count - 1) % MAX_HISTORY]);
                printf("%s\n", input);
            } 
            else {
                // retrieve Nth command
                int n = atoi(&input[1]);
                int minNumber = (history_count - MAX_HISTORY > 0) ? history_count - MAX_HISTORY : 0;
                if (n <= minNumber || n > history_count) {
                    printf("No such command in history.\n");
                    continue;
                }
                strcpy(input, history[(n - 1) % MAX_HISTORY]);
                printf("%s\n", input);
            }
        } 
        else if (strcmp(input, "history") == 0) {
            // display command history
            int start = history_count - MAX_HISTORY;
            int iterations = 0;
            if (start < 0) start = 0;
            for (int i = history_count - 1; i >= start; i--) {
                printf("%d %s\n", history_count - iterations, history[i % MAX_HISTORY]);
                iterations++;
            }

            continue;
        }

        // add_to_history(input);

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
            execvp(args[0], args);
            return -1;
        } 
        else {
            if (!hasAmpersand) {                    // parent process, waits on child if no &
                int status;
                waitpid(rc, &status, 0);
                if (WEXITSTATUS(status) == 255) {
                    fail = -1;
                }
            }
            else {
                hasAmpersand = 0;
            }
            for(int i = 0; i < MAX_LINE/2 + 1; i++) {
                args[i] = NULL;
            }
        }

        if (fail != -1) add_to_history(inputCopy);
        else fail = 0;
        /**
        * After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) if command included &, parent will invoke wait()
        */
    }
return 0;
}