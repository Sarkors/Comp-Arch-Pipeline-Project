#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char* argv[]) {
    // declare pipes
    int fd1[2], fd2[2]; //pipes, fd1: parent to child1, fd2: child1 to child2
    pid_t pid;

    if (pipe(fd1) == -1 || pipe(fd2) == -1) { //creating two pipes
        fprintf(stderr, "Pipe failed"); //fprintf just specifies the stream this case stderr
        return 1;
    }

    pid = fork(); //forks to create child process
    if (pid < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (pid > 0) { // Parent process
        close(fd1[READ_END]); //parent closes read end to write to fd1
        close(fd2[WRITE_END]); //parent closes write end of fd2 because it wont write there

        FILE* fp = fopen("input.txt", "r"); //opens input text to read
        if (fp == NULL) {
            fprintf(stderr, "Failed to open file\n");
            exit(1);
        }

        char line[BUFSIZ]; //reads input line by line and writes to fd1
        while (fgets(line, BUFSIZ, fp)) {
            write(fd1[WRITE_END], line, strlen(line));
            memset(line, 0, BUFSIZ);
        }

        fclose(fp); //closes the pipe write end and waits for fd1 and fd2 
        
        close(fd1[WRITE_END]);  
        wait(NULL);  
        close(fd2[READ_END]);
        wait(NULL);  
    } 
    else { // Child processes (if the process is a child, forks again to make another child)
        pid = fork();

        if (pid > 0) { // Second process (if this is the first child)
            close(fd1[WRITE_END]);
            close(fd2[READ_END]);

            // reads from fd1 and converts uppercase to lowercase
            char line[BUFSIZ];
            while (read(fd1[READ_END], line, BUFSIZ) > 0) {
                for (int i = 0; i < strlen(line); i++) {
                    // Change lower to upper and upper to lower cases
                    // write code here
                    if (islower(line[i])) line[i] = toupper(line[i]);
                    else if (isupper(line[i])) line[i] = tolower(line[i]);
                }
                //writes changed text to fd2 
                write(fd2[WRITE_END], line, strlen(line));
                memset(line, 0, BUFSIZ);
            }
            //closes pipes
            close(fd1[READ_END]);
            close(fd2[WRITE_END]);
            exit(0);

        } 
        else { // Third process (if its the second child)
            // Close unused pipes
            // write code here
            close(fd1[READ_END]);
            close(fd1[WRITE_END]);
            close(fd2[WRITE_END]); 
            

            
            FILE *out_fp = fopen("output.txt", "w");

            if (out_fp == NULL) {
                perror("Error opening output.txt");
                exit(1);
            }

            char line[BUFSIZ];
            
            while (read(fd2[READ_END], line, BUFSIZ) > 0) {
                // prints to the output file

                // write code here
                fprintf(out_fp, "%s", line); 
            }
            //closes the file and pipe
            fclose(out_fp);
            
            close(fd2[READ_END]);
            
            exit(0);
        }
    }

    return 0;
}