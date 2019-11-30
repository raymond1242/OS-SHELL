#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SHELL_RL_BUFFSIZE 1024
#define SHELL_TOK_BUFFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"
#define MAXLIST 100

int shell_execute(char ** args){
    pid_t pid;
    int status;
    int position = 0;
    while(args[position]){
	position++;
    }
    if(args[0] == NULL){
        return 1;
    }
    else if(strcmp(args[0], "chau") == 0){
        printf("GOOD BYE ! \n");
        return 0;
    }
    else if(strcmp(args[0], "history") == 0){
        printf("No implementado aun\n");
        return 1;
    }
    else if(strcmp(args[0], "cd") == 0){
        chdir(args[1]);
        return 1;
    }
    else if(strcmp(args[0], "ayuda") == 0){
        printf(" -------- Esta es la Ayuda UCSP --------\n");
        printf(" Comandos :\n");
        printf(" > ls      Muestra archivos en el directorio que estas\n");
        printf(" > df      Muestra informaciÃn del espacio total, ocupado y libre del sistema\n");
        printf(" > pwd     Muestra la direccion en la que estas\n");
        printf(" > w       Muestra los usuarios que estan conectados y sus procesos\n");
        printf(" > ps      Muestra procesos actuales\n");
        printf(" > date    Muestra la fecha actual\n");
        printf(" > cat     Muestra la informacion contenida de un archivo \n");
        printf(" > nano    Crea archivos en el directorio actual\n");
        printf(" > cd      Cambia de directorio\n");
        printf(" > chau    Sale del Shell\n\n");
        return 1;
    }
    if(strcmp(args[position-1], "&") == 0){
        printf("Entro el proceso en BackGround \n");
        args[position-1] = NULL;
        pid = fork();
        if(pid == 0){
            if(execvp(args[0], args) == -1){
                perror("No existe comando en Shell, use comando Ayuda\n");
            }
            exit(EXIT_FAILURE);
        }
        else if(pid < 0){
            perror("Shell");
        }
        //return 1;
    }
    else{
        pid = fork();
        if(pid == 0){
            
            if(execvp(args[0], args) == -1){
                perror("No existe comando en Shell, use comando Ayuda\n");
            }
            exit(EXIT_FAILURE);
        }
        else if(pid < 0){
            perror("Shell");
        }
        else{
            do{
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
    return 1;
}

char **shell_split_line(char *line){
    int buffsize = SHELL_TOK_BUFFSIZE, position = 0;
    char **tokens = malloc(buffsize * sizeof(char*));
    char *token,**tokens_backup;
    if(!tokens){
        fprintf(stderr,"Shell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, SHELL_TOK_DELIM);
    while(token != NULL){
        tokens[position] = token;
        position++;
        if(position >= buffsize){
            buffsize += SHELL_TOK_BUFFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, buffsize * sizeof(char*));
            if(!tokens){
                free(tokens_backup);
                fprintf(stderr,"Shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token=strtok(NULL,SHELL_TOK_DELIM);
    }
    tokens[position]=NULL;
    return tokens;
}

char *shell_read_line(){
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    //historial[id] = line;
    //id++;
    
    return line;
}

int execArgsPiped(char** parsed, char** parsedpipe) {
    // 0 is read end, 1 is write end 
    int status;
    int pipefd[2];  
    pid_t p1, p2; 
    if (pipe(pipefd) < 0) { 
        printf("\nPipe could not be initialized"); 
        return; 
    }
    p1 = fork(); 
    if (p1 < 0) { 
        printf("\nCould not fork"); 
        return; 
    }
    else if (p1 == 0){
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCould not execute command 1.."); 
        }
        exit(EXIT_FAILURE); 
    }
    else { 
        // Parent executing 
        p2 = fork(); 
        if (p2 < 0){ 
            printf("\nCould not fork"); 
            return 1;
        }
        else if (p2 == 0){ 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("\nCould not execute command 2.."); 
                exit(EXIT_FAILURE); 
            }
        }
        else { 
            // parent executing, waiting for two children 
            do{
                waitpid(p1, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        } 
    }
    return 1;
}

void shell_loop(){
    char *line;
    char *a, *b, *temp;
    char **args,**argsPipe;
    int status;
    //int exeflag = 0;
    do{
        printf(" > ");
        line = shell_read_line();
        temp = line;
        a = strsep(&temp, "|");
        b = strsep(&temp, "|");
        //printf("-%s-\n", a);
        //printf("-%s-\n", b);
        if(b){
            args = shell_split_line(a);
            argsPipe = shell_split_line(b);
            //printf("-%s-", args[0]);
            //printf("-%s-", argsPipe[0]);
            //printf("-%s-", argsPipe[1]);
            status = execArgsPiped(args, argsPipe);
        }
        else{
            args = shell_split_line(line);
            status = shell_execute(args);
        }
        free(line);
        free(args);
    }while(status);
}

int main()
{
    shell_loop();
    return EXIT_SUCCESS;
}