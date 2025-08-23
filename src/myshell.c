#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


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