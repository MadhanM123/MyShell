#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define SHELL_BUFSZ 1024
#define SHELL_TOK_BUFSZ 64
#define SHELL_TOK_DELIM " \t\r\n\a"

// Built-in Shell Command Forward Declarations

int shell_cd(char** args);
int shell_help();
int shell_exit();

// Built-in Shell Command Names

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

int shell_num_builtins(){
    return sizeof(builtin_str) / sizeof(char*);
}

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


int main(int argc, char** argv){
    //Read and execute configuration files for shell(MyShell doesn't have any)

    shell_loop(); //Command loop

    //Exceute shutdown commands, free memory, and terminate(MyShell doesn't have a shutdown command)

    return EXIT_SUCCESS;
}