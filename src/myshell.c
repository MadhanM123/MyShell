#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SHELL_BUFSZ 1024
char* shell_read_line(void){
    int bufsz = SHELL_BUFSZ;
    int pos = 0;
    char* buf = malloc(sizeof(char) * bufsz);
    int c;

    if(!buf){
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        c = getchar();

        if(c == EOF || c == '\n'){
            buf[pos] = '\0';
            return buf;
        }
        else{
            buf[pos] = c;
        }
        pos++;

        if(pos >= bufsz){
            bufsz += SHELL_BUFSZ;
            buf = realloc(buf, bufsz);
            if(!buf){
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


void shell_loop(void){
    char* line;
    char** args;
    int status;

    do {
        printf("> ");
        line = shell_read_line();
        args = shell_parse_line();
        status = shell_exec(args);

        free(line);
        free(args);
    } while(status);
}


int main(int argc, char** argv){
    //Read and execute configuration files for shell(MyShell doesn't have any)

    shell_loop(); //Command loop

    //Exceute shutdown commands, free memory, and terminate(MyShell doesn't have a shutdown command)

    return EXIT_SUCCESS;
}