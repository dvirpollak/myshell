//dvir pollak 318528627
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


#define INPUT_SIZE 100
#define SPLIT_TOKEN " "

void DisplayPrompt();
char **splitArgv(char *input, char **argv,int *sizeOfArgv);
void freeAll(char** argv,int sizeOfArgv);
void setEnv(char *const *argv);

typedef struct commend {
    char input[100];
    pid_t pid;
}commend;

static struct commend history[100];

int main(int argc, char *argv[]) {
    int sizeOfArgv=0;
    char **inArgv;
    char input[INPUT_SIZE]="";
    char copyInput[INPUT_SIZE];
    setEnv(argv);
    int historyIndex=0;
    char cwd[INPUT_SIZE];


    while (1) {
        DisplayPrompt();
        fgets(input, INPUT_SIZE, stdin);
        strcpy(copyInput,input);
        inArgv = splitArgv(input, inArgv, &sizeOfArgv);


        if (strcmp("cd", inArgv[0]) == 0) {
            char *currentPath = getcwd(cwd, sizeof(cwd));
            if (inArgv[1] != NULL) {
                char *pathToChange = inArgv[1];
                int error = chdir(pathToChange);
                if(error!=0)
                    perror("chdir failed");

            }

            history[historyIndex].pid = getpid();
            strcpy(history[historyIndex].input, copyInput);

        } else if (strcmp("history", inArgv[0]) == 0) {
            history[historyIndex].pid = getpid();
            strcpy(history[historyIndex].input, copyInput);
            int i;
            for ( i = 0; i < historyIndex + 1; ++i) {
                printf("%d %s", history[i].pid, history[i].input);
            }

        }
        else if (strcmp("exit",inArgv[0])==0){
            exit(1);
    }else{

          pid_t pid;
          pid = fork();
          if(pid<0){
              perror("fork failed");
          }
          if(pid==0){
              int error=execvp(inArgv[0], inArgv);
              if (error==-1){
                  perror("execvp failed");
                  exit(error);
              }
          } else{
              history[historyIndex].pid= pid;
              strcpy(history[historyIndex].input,copyInput);
              wait(&pid);
          }
      }
        historyIndex++;
        memset(input, 0, sizeof input);
        freeAll(inArgv,sizeOfArgv);

    }

    return 0;
}

void setEnv(char *const *argv) {
    char *old = getenv("PATH");
    int i=0;
    while (argv[i]!=NULL){
        strcat(old,":");
        strcat(old,argv[i]);
        i++;
    }
    setenv("PATH",old,1);
}

void freeAll(char** inArgv,int sizeOfArgv)
{
    int i;
    for (i = 0; i < sizeOfArgv; ++i) {
        free(inArgv[i]);
         inArgv[i]=NULL;
    }
    free(inArgv);
    inArgv=NULL;
}

void DisplayPrompt() {
    printf("$ ");
    fflush(stdout);
}

char **splitArgv(char *input, char **inArgv, int *sizeOfArgv ) {
    int i=0,counter=0;
    char buffer[INPUT_SIZE];

    strcpy(buffer,input);

    char* ptr= strtok(input,SPLIT_TOKEN);
    while(ptr!=NULL)
    {
        if(*ptr !='\n'){
            counter++;
        }

        ptr=strtok(NULL,SPLIT_TOKEN);


    }
    inArgv = (char**)malloc((counter+1)*(sizeof(char*)));
    if(inArgv==NULL)
    {
        perror("malloc failed");
    }

    char* ptrCopy= strtok(buffer,SPLIT_TOKEN);
    while(ptrCopy!=NULL)
    {

        inArgv[i]=(char*)malloc((sizeof(char)+1)*strlen(ptrCopy));
        if(inArgv[i]==NULL)
        {
            int j;
            for (j = i-1; j >-1 ; j--) {
                free(inArgv[j]);
            }
            free(inArgv);
            perror("malloc failed");
        }
        if(counter==i+1){
            ptrCopy =strtok(ptrCopy,"\n");
        }
        if(ptrCopy!=NULL){
            strcpy(inArgv[i],ptrCopy);
            inArgv[i][strlen(ptrCopy)]='\0';
            ptrCopy=strtok(NULL,SPLIT_TOKEN );
        }


        i++;
    }
    (*sizeOfArgv)=counter;
    return inArgv;

}

