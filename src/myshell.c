#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define SHELL_BUFSZ 1024 /**< Buffer size for reading input lines */
#define SHELL_TOK_BUFSZ 64 /**< Initial buffer size for parsing tokens */
#define SHELL_TOK_DELIM " \t\r\n\a" /**< Delimiters for tokenizing input */

/*----------------------------------------------------------------*/
/* Built-in Shell Command Declarations                            */
/*----------------------------------------------------------------*/


/**
 * @brief Changes the current working directoy
 * 
 * @param args Null-terminated array of arguments. args[0] is "cd", args[1] is target directory.
 * @return int 1 to continue executing, 0 to terminate the shell.
 */
int shell_cd(char** args);


/**
 * @brief Displays helpful information about the shell and it's built-in commands.
 * 
 * @return int 1 to continue executing
 */
int shell_help();

/**
 * @brief Exits the shell.
 * 
 * @return int 0 to terminate.
 */
int shell_exit();

/*----------------------------------------------------------------*/
/* Built-in Shell Command Names and Function Pointers             */
/*----------------------------------------------------------------*/

char* builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char**) = {
    &shell_cd,
    &shell_help,
    &shell_exit
};


/**
 * @brief Gets number of built-in commands.
 * 
 * @return int Number of built-ins in builtin_str[]
 */
int shell_num_builtins(){
    return sizeof(builtin_str) / sizeof(char*);
}

/*----------------------------------------------------------------*/
/* Built-in Shell Command Implementations                         */
/*----------------------------------------------------------------*/
int shell_cd(char** args){
    if(args[1] == NULL){
        fprintf(stderr, "shell: expected argument to cd\n");
    }
    else{
        if(chdir(args[1]) != 0){
            perror("shell");
        }
    }
    return 1;
}

int shell_help(){
    printf("Madhan's Shell\n");
    printf("Type program names and arguments, and enter.\n");
    printf("The following are built in:\n");

    for(int i = 0; i < shell_num_builtins(); i++){
        printf(" %s\n", builtin_str[i]);
    }

    printf("Use the man command to learn information about other programs.\n");
    return 1;
}

int shell_exit(){
    return 0;
}

/*----------------------------------------------------------------*/
/* Shell Execution Functions                                      */
/*----------------------------------------------------------------*/

/**
 * @brief Launches program by forking and calling execvp from child.
 * 
 * @param args Null-terminated array of command arguments.
 * @return int 1 to continue execution
 */
int shell_launch(char** args){
    __pid_t pid;
    int status;

    pid = fork();
    if(pid == 0){
        if(execvp(args[0], args) == -1){
            perror("shell");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid < 0){
        perror("shell");
    }
    else{
        do{
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

/**
 * @brief Executes commands that are built-in, otherwise calls shell_launch()
 * 
 * @param args Null-terminated array of command arguments
 * @return int 0 if shell should exit, 1 if shell should continue execution
 */
int shell_exec(char** args){
    if(args[0] == NULL){
        return 1;
    }

    for(int i = 0; i < shell_num_builtins(); i++){
        if(strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }

    return shell_launch(args);
}

/*----------------------------------------------------------------*/
/* Shell Input Processing Functions                               */
/*----------------------------------------------------------------*/

/**
 * @brief Reads a line of input from stdin
 * 
 * @return char* Pointer to the read line
 */
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

/**
 * @brief Parses line of input into tokens
 * 
 * @param line Input line
 * @return char** Null-terminated array of token
 */
char** shell_parse_line(char* line){
    int bufsz = SHELL_BUFSZ, pos = 0;
    char** toks = malloc(bufsz * sizeof(char*));
    char* tok, **toks_back;

    if(!toks){
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    tok = strtok(line, SHELL_TOK_DELIM);
    while(tok != NULL){
        toks[pos] = tok;
        pos++;

        if(pos >= bufsz){
            bufsz += SHELL_TOK_BUFSZ;
            toks_back = toks;
            toks = realloc(toks, bufsz * sizeof(char*));
            if(!toks){
                free(toks_back);
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        tok = strtok(NULL, SHELL_TOK_DELIM);
    }

    toks[pos] = NULL;
    return toks;
    
}

/*----------------------------------------------------------------*/
/* Shell Control Loop Functions                                   */
/*----------------------------------------------------------------*/

/**
 * @brief Main shell loop
 * Continuously:
 * - Prints prompt
 * - Reads input 
 * - Tokenizes input
 * - Executes command
 * - Frees memory
 */
void shell_loop(void){
    char* line;
    char** args;
    int status;

    do {
        printf("> ");
        line = shell_read_line();
        args = shell_parse_line(line);
        status = shell_exec(args);

        free(line);
        free(args);
    } while(status);
}

/**
 * @brief Program entry point
 * 
 * @param argc Argument count(unused)
 * @param argv Argument vector(unused)
 * @return int EXIT_SUCCESS when shell terminates
 */
int main(int argc, char** argv){
    //Read and execute configuration files for shell(MyShell doesn't have any)

    shell_loop(); //Command loop

    //Exceute shutdown commands, free memory, and terminate(MyShell doesn't have a shutdown command)

    return EXIT_SUCCESS;
}