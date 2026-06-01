#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "sys/types.h"
#include "string.h"
#include "string.h"
#include "unistd.h"
#include "sys/wait.h"
#include "signal.h"

enum Shell_state {WAITING_INPUT,SPLIT_LINE,EXECUTE,EXIT};

typedef struct {
    char *input;
    char **args;
    enum Shell_state next_state;
}Shell_input;

//function declearation
int cd(char **args, Shell_input *input_status);
int help(char **args, Shell_input *input_status);
int shell_exit(char **args, Shell_input *input_status);
void clean_up(Shell_input *input_status);

//build in command
char *command[] = {"cd", "help","exit"};
int (*builtin_func[])(char **, Shell_input *) = {
    &cd,
    &help,
    &shell_exit
};
int command_num = sizeof(command)/sizeof(char *);


int cd(char **args, Shell_input *input_status){
    if(args[1] != NULL){
        chdir(args[1]);
    }else{
        fprintf(stderr,"please input valid route");
    }
    return 0;
}

int help(char **args , Shell_input *input_status){
    printf("This is Daniel's shell\n");
    printf("The following are build in commands\n");
    printf(" -cd\n -help\n -exit\n");
    return 0;
}

int shell_exit(char **args, Shell_input *input_status){
    input_status->next_state = EXIT;

    return 0;
}

void clean_up(Shell_input *input_status){
    clearerr(stdin);

    if(input_status->input != NULL){
        free(input_status->input);
        input_status->input = NULL;
    }

    if(input_status->args != NULL){
        free(input_status->args);
        input_status->args = NULL;
    }
}

int subpros(Shell_input *input_status){
    pid_t pid = fork();
    char **args = input_status->args;

    if(pid<0){
        perror("fork");
    }else if(pid == 0){
        if(execvp(args[0],args) == -1){
            exit(EXIT_FAILURE);
        }

    }else{
        int status;
        waitpid(pid,&status,0);
    }

    return 0;
}

int execute(Shell_input *input_status){

    char **args = input_status->args;

    for(int i=0;i<command_num;i++){
        if(strcmp(args[0],command[i])==0){

            return builtin_func[i](args, input_status);
        }
    }


    return subpros(input_status);

}

int split_line(Shell_input *input_status){
    size_t buf_size=64;
    char **tokens=malloc(buf_size*sizeof(char *));
    char *token;

    if(tokens == NULL){
        perror("malloc");
        return 1;
    }

    const char *delim = {" \n\t\f\a"};
    int position =0;

    token = strtok(input_status->input, delim);

    if(token == NULL){
        fprintf(stderr, "invalid command\n");
        free(tokens);
        input_status->args = NULL;
        return 1;
    }

    while(token != NULL){
        if (*token == -1)
        {
            break;
        }else{
            tokens[position] = token;
        }

        token = strtok(NULL,delim);
        position++;

        if(position>=buf_size){
            buf_size += buf_size;
            char **new_tokens = realloc(tokens, buf_size*sizeof(char *));
            if(tokens == NULL){
                perror("realloc");
                free(tokens);
                exit(EXIT_FAILURE);
            }

            tokens = new_tokens;
        }
    }

    tokens[position] = NULL;
    input_status->args = tokens;

    return 0;
}

int get_input(Shell_input *input_status){
    char *line = NULL;
    size_t buf_size = 0;
    ssize_t words = getline(&line,&buf_size,stdin);

    if(words ==-1){
        free(line);
        printf("\n");
        return -1;
    }else if(*line == '\n'){
        printf("");
        free(line);
        return 1;
    }

    input_status->input = line;

    return 0;
}

int main(void){
    bool running = true;
    char cwd[1024];

    Shell_input *input_status = malloc(sizeof(Shell_input));

    if (input_status == NULL){
        perror("malloc");
    }

    input_status->next_state = WAITING_INPUT;

    do{
        switch(input_status->next_state){
            case WAITING_INPUT:
                getcwd(cwd,sizeof(cwd));
                printf("%s >",cwd);

                int input = get_input(input_status);

                if(input == 0 ){
                    input_status->next_state = SPLIT_LINE;
                }else if(input == 1){
                    clean_up(input_status);
                    input_status->next_state = WAITING_INPUT;
                }else{
                    input_status->next_state = EXIT;
                }

                break;
            case SPLIT_LINE:

                int split = split_line(input_status);

                if ( split == 0){
                    input_status->next_state = EXECUTE;
                }else if(split == 1){
                    clean_up(input_status);
                    input_status->next_state = WAITING_INPUT;
                }

                break;
            case EXECUTE:
                if(execute(input_status) == 0){

                    if(input_status->next_state != EXIT){
                        clean_up(input_status);
                        input_status->next_state = WAITING_INPUT;
                    }

                }else{
                    clean_up(input_status);
                    input_status->next_state = WAITING_INPUT;
                }

                break;
            case EXIT:
                printf("exit sucess\n");
                clean_up(input_status);
                running = false;
            }
    }while(running);

    free(input_status);

    return 0;

}

/*
What I learned?
- Finite State Machine :using switch case statement to manage and change application running in corresponding state
-
*/