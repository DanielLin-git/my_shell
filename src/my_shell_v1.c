#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"

const int command_num =3;
char *buildin_command[] ={
    "cd","help","exit"
};

int my_shell_cd(char **args);
int my_shell_help(char **args);
int my_shell_exit(char **args);

int (*buildin_func[])(char **) = {
    &my_shell_cd,
    &my_shell_help,
    &my_shell_exit
};

int my_shell_cd(char **args){
    if(args[1] == NULL){
        fprintf(stderr,"please reenter a path");
    }else{
        if(chdir(args[1])!= 0){
            fprintf(stderr,"please enter a valid path\n");
        }
    }
    return 1;
}

int my_shell_help(char **args){
    int i;
    printf("-this is Daniel's shell\n");
    printf("-the following is builtin\n");
    for(i=0;i<command_num;i++){
        printf("  %s\n",buildin_command[i]);
    }
    return 1;
}

int my_shell_exit(char **args){
    return 0;
}




char *read_line(){
    size_t buf_size = 1024;
    char *buf = malloc(sizeof(char)*buf_size);
    int position = 0;

    if(buf == NULL){
        fprintf(stderr,"fail to allocate memory");
        exit(EXIT_FAILURE);
    }

    while(1){

        int c = getchar();

        if(c == EOF || c == -1){
            exit(EXIT_SUCCESS);
        }else if(c == '\n'){
            buf[position] = '\0';
            return buf;
        }else{
            buf[position] = c;
        }
        position ++;

        if(position>=buf_size){
            buf_size += buf_size;
            buf = realloc(buf,buf_size);

            if(buf == NULL){
                fprintf(stderr,"realloc memory failure");
                exit(EXIT_FAILURE);
            }

        }
    }
}


char **split_line(char *line){
    int buf_size =64;
    int position=0;
    char *delim = " \n\a\t\r";
    char **tokens = malloc(buf_size * sizeof(char *));
    char *token = strtok(line,delim);

    if(tokens == NULL){
        fprintf(stderr,"memory allocation failure");
        exit(EXIT_FAILURE);
    }

    if(token == NULL){
        fprintf(stderr,"there is no valid delim in string\n");
        return tokens;
    }

    while(token != NULL){

        tokens[position] = token;

        position++;

        if(position>=buf_size){
            buf_size += buf_size;
            char **tmp =realloc(tokens,buf_size*sizeof(char *));
            if(tmp ==NULL){
                free(tokens);
                fprintf(stderr,"memory allocation failure");
                exit(EXIT_FAILURE);
            }
            tokens = tmp;
        }

        token = strtok(NULL, delim);

    }
    tokens[position] = NULL;
    return tokens;
}

int subpros(char **args){
    pid_t pid;
    pid = fork();
    int status;
    if(pid <0){
        fprintf(stderr,"can't initialize sub process");

    }else if(pid == 0){
        if(execvp(args[0],args) ==-1){
            fprintf(stderr,"execute %s error",args[0]);
        }
        exit(EXIT_FAILURE);
    }else{
        waitpid(pid,&status,WUNTRACED);
        return 1;
    }

}
int execute(char **args){
    int i;
    if(args[0] == NULL){
        return 1;
    }

    for(i=0;i<command_num;i++){
        if(strcmp(args[0],buildin_command[i]) == 0){
            return buildin_func[i](args);
        }

    }

    return subpros(args);
}

void shell_loop(){

    char *line=NULL;
    char **args=NULL;
    int status;

    do {
        printf(">");
        line = read_line();
        args = split_line(line);
        status = execute(args);
        free(line);
        free(args);
    }while (status);

}

int main(int argc, char **argv){

    shell_loop();

    return 0;
}