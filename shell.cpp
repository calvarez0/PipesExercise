/****************
LE2: Introduction to Unnamed Pipes
****************/
#include <stdio.h> // perror
#include <stdlib.h> // exit, EXIT_FAILURE
#include <unistd.h> // pipe, fork, dup2, execvp, close
#include <string.h> // memcpy
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main (int argc, char **argv) {
    char p_name[256];  // process name, will store process name from argv[1] given as user input
    if (argc != 2) {        // Add the correct condition; hint: what should the value of argc be?
    // this checks if the user input the correct amount of variables (needs to be 2)
        fprintf(stderr, "Incorrect format. Use ./shell <process name>\n");
        exit(EXIT_FAILURE);
    }

    // copy argv process name into p_name; copy memory instead of strcpy
    memcpy(p_name, argv[1], sizeof(argv[1]));  // no idea why we had to do this it was just in skeleton code
    // p_name is the destination, argv[1] (process name) is the source, and sizeof(argv[1]) is the size obv
    
    // lists all the processes
    // we can list all the processes using ps command

    // create a pointer to an array for execvp, say cmd1
    // these are the arguments for args in execvp
     // this is for the one that runs first, outputs list of processes 
    
    // create a pointer to an array for execvp, say cmd2
      // this one runs grep with the user input

    // create pipe before forking
    /* you need to create the pipe before forking so that the children can
     reference the same pipe */
    int fd[2];
    if(pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    } 
    // fd[0] is read end, fd[1] is write end

    // pipe one command to another
    // start by forking
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // if it is a child, fork again to run the commands
    if (pid == 0) {

        close(fd[0]); // close read end since we're not using it

        /* this makes it so that instead of listing processes on terminal, it is outputted
        to the write end of the pipe, where it will be send to the grandchild process */
        dup2(fd[1], STDOUT_FILENO);

        close(fd[1]); // close write end since it's no longer needed after dup2()
        const char *cmd1[] = {"ps", "aux", NULL};
        execvp(cmd1[0], const_cast<char**>(cmd1));  // runs ps
        perror("execvp ps");
        exit(EXIT_FAILURE);


        // now you have the current process, its child, and grandchild process
        // which of the child or the grandchild exec cmd1? the other runs cmd2

        // The output of which command act as the input of the other?
        // Use dup2() to redirect outputs and inputs
        // Make sure to close the unused sides with close(fd[0]) or close(fd[1])
        
        } else {
            pid_t gpid = fork();  // granchild pid
            if (gpid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (gpid == 0) { // checking to see if we're in grandchild and not parent (child)

                close(fd[1]); // close write end of pipe since we're only reading
                dup2(fd[0], STDIN_FILENO);  /* this makes it so that the read end of the pipe takes in
                the standard input (which will be the ps) */
                close(fd[0]);  // don't need read end after changing it to standard in
                const char *cmd2[] = {"grep", p_name, NULL};
                execvp(cmd2[0], const_cast<char**>(cmd2));
                perror("execvp grep");
                exit(EXIT_FAILURE);
            } else {
                close(fd[0]);  // close pipe from parent
                close(fd[1]);  // close wrie from parent
                waitpid(pid, NULL, 0); // wait for chil
                waitpid(gpid, NULL, 0); // wati for gchild
            }
        }
    }
