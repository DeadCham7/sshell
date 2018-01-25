#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_ARGS 16
#define MAX_SIZE 512

int parseThenExecute(char *cmdl);
struct daemonComLL* backGroundProcessList;
char* concat(const char *string1, const char *string2)
{
    char *result = (char*)malloc(strlen(string1)+strlen(string2)+1);
    strcpy(result, string1);
    strcat(result, string2);
    return result;
}
//void pipe();
struct Com
{
    char *pos;
    char *argv[MAX_ARGS];
    char *cmdnospace[MAX_ARGS];
    char *output;
};

struct daemonCom{
    struct daemonCom* next;
    int pid;
    char *cmd;
};
struct daemonComLL {
    int listLen;
    struct daemonCom* head;
};
void insertBCcom(struct daemonComLL* list, struct daemonCom* element) {
    struct daemonCom* rest = list->head;
    element->next = rest;
    list->head = element;
    list->listLen++;
}
struct daemonCom* getBC(struct daemonComLL* list) {
    struct daemonCom* current = list->head;
   
    return current;
}

void exit_command(char* command)
{
    if (strcmp(command, "exit") == 0)
    {
        fprintf(stderr, "Bye...\n");
        exit(0);
    }
}

void pwd_command(char* command)
{
    char directory[512];
    
    if (strcmp(command, "pwd") == 0)
    {
        getcwd(directory, sizeof(directory));
        fprintf(stdout, "%s\n", directory);
    }
}

void cd_command(char** cmdWithArg)
{
    char newPath[512];
    
    getcwd(newPath, sizeof(newPath));
    
    if (strcmp(cmdWithArg[0], "cd") == 0 && strcmp(cmdWithArg[1], "..") == 0)
    {
        chdir("..");
    }
    else if(strcmp(cmdWithArg[0], "cd") == 0 && strcmp(cmdWithArg[1], "..") != 0)
    {
        strcat(newPath, "/");
        strcat(newPath, cmdWithArg[1]);
        if(chdir(newPath) == -1)
        {
            fprintf(stderr, "Error: no such directory\n");
            exit(1);
        }
    }
}
/*
void pipe(){
    //parse for piping
	   int j = 0;
	   char* cmdWithArg2[512] = { NULL };
    
	   char *n = strtok(command, "|");
	   while (n != NULL)
       {
           cmdWithArg2[j++] = n;
           n = strtok(NULL, "|");
       }
    
	   
	   int fd[2];
	   
	   //start piping
	   pipe(fd);
	   pid = fork();
	   
	   if (pid == 0)
       {
           if(strchr(command, '|') != NULL)
           {
               close(fd[1]);
               dup2(fd[0], STDOUT_FILENO);
               close(fd[0]);
               executeCommandfromChild(cmdWithArg2[0]);
               exit(1);
           }
           executeCommandfromChild(cmdl);
           exit(1);
       }
       else if(pid > 0)
       {
           pid = fork();
           
           if(pid == 0)
           {
               dup2(fd[1], STDIN_FILENO);
               close(fd[0]);
               close(fd[1]);
               executeCommandfromChild(cmdWithArg2[1]);
               exit(1);
           }
           else
           {
               //built in pwd command
               pwd_command(path);
               
               waitpid(pid, &status, 0);
               
               fprintf(stderr, "+ completed '%s' [%d]\n", cmdl, WEXITSTATUS(status));
           }
       }
       else
       {
           perror("fork");
       }

}*/

int inputOrOutputRedir(char* cmdl, char* fileName){
    const char inputChar = '<';
    const char outputChar = '>';
    const char whiteSpace = ' ';
    int len = strlen(cmdl);
    int i;
    for(i = 1; i < len; i++) {
        if(cmdl[i] == outputChar) {
            char checkWhitespace = cmdl[i+1];
            //printf("Am I whitespace? : '%c', i: '%d'", checkWhitespace, i);
            if(checkWhitespace == whiteSpace) i++;
            
            //printf("Am I whitespace? : '%c', i: '%d'", checkWhitespace, i);
            int c = 0;
            while (c < len-i) {
                fileName[c] = cmdl[i+c+1];
                c++;
            }
            fileName[c] = '\0';
            //printf("1cmdl: '%s'\n",cmdl);
            cmdl = strchr(cmdl, '>');
            if (cmdl != NULL) {
                *cmdl = '\0';
            }
            
            //printf("2cmdl: '%s'\n",cmdl);
            return 1;
        }
        if(cmdl[i] == inputChar) {
            char checkWhitespace = cmdl[i+1];
            //printf("Am I whitespace? : '%c', i: '%d'", checkWhitespace, i);
            if(checkWhitespace == whiteSpace) i++;
            
            //printf("Am I whitespace? : '%c', i: '%d'", checkWhitespace, i);
            int c = 0;
            while (c < len-i) {
                fileName[c] = cmdl[i+c+1];
                c++;
            }
            fileName[c] = '\0';
            //printf("1cmdl: '%s'\n",cmdl);
            cmdl = strchr(cmdl, '<');
            if (cmdl != NULL) {
                *cmdl = '\0';
            }
            
            
            return 0;
        }
    }
    return 2;
};

void executeCommandfromChild(char *cmdl){   // todo: work on redir command parser/fd
    //printf("imChild");
    char *fileName = (char*)malloc(100);
    int zeroForinputOneForOutPut=inputOrOutputRedir(cmdl, fileName);
    size_t length = strlen(cmdl);
    if (length > 0 && cmdl[length-1] == ' ') {
        cmdl[--length] = '\0';
    }
    int fileDescriptor;
    
    if(zeroForinputOneForOutPut==0){   //a boolean function that check if it has input redir
        //printf("im input");
        //parse filename
        
        if (strlen(fileName) == 0) {
            printf("Error: No file name");
            return;
        }
        
        fileDescriptor = open(fileName, O_RDONLY);
        //printf("fd: %d", fileDescriptor);
        if (fileDescriptor == -1) {
            printf("Error: '%s' failed to open",fileName);
        } else {
            dup2(fileDescriptor, 0);
            close(fileDescriptor);
        }
        parseThenExecute(cmdl);
        
    }else if(zeroForinputOneForOutPut==1){
        
        //printf("3cmdl: '%s'\n", cmdl);
        if (strlen(fileName) == 0) {
            printf("Error: File name empty");
            return;
        }
        
        
        fileDescriptor = open(fileName, O_CREAT | O_WRONLY);
        
        if (fileDescriptor == -1) {
            printf("Error: file, '%s', failed to open",fileName);
            
        } else {
            dup2(fileDescriptor, 1);
            close(fileDescriptor);
        }
        parseThenExecute(cmdl);
        //close stdoutput
        //assign a file to file descripter 1
        //execute the commend with parsed command that does not have redir
        //restore the stdout with dup2
    }else{
        parseThenExecute(cmdl);
    }
}


int parseThenExecute(char *cmdl){
    //char* args[3]= { NULL };
    
    int i = 0;
    char* cmdWithArg[3]= { NULL };
    
    char *p = strtok (cmdl, " ");
    while (p != NULL){
        cmdWithArg[i++] = p;
        p = strtok (NULL, " ");
    }
    
    cd_command(cmdWithArg);
    
    if(strcmp(cmdWithArg[0], "pwd") == 0 || strcmp(cmdWithArg[0], "cd") == 0)
    {
        exit(0);
    }
    
    if(execvp(cmdWithArg[0], cmdWithArg)<0){
        fprintf(stderr, "Error: command not found\n");
        exit(1);
    }
    return 0;
};

int main(int argc, char *argv[])
{
    //    int bgi=0;
    backGroundProcessList = malloc(sizeof(struct daemonComLL));
    bool daemon;
    while(1){
        
        
        fflush (stdin);
        int status;
        waitpid(-1, &status, WNOHANG);
        char *cmdl = (char*)malloc(512);
        size_t len = 512;
        
        printf("sshell$ ");
        
        fgets(cmdl, len, stdin);
        
        if (strcmp(cmdl, "\n") == 0) { //if no command
            checkBGcomplete();
            continue;
        }
        
        size_t length = strlen(cmdl);
        if (length > 0 && cmdl[length-1] == '\n') {
            cmdl[--length] = '\0';
        }
        exit_command(cmdl);
        size_t lengthForBg = strlen(cmdl);
        if (lengthForBg > 0 && cmdl[lengthForBg -1] == '&') {
            cmdl[--lengthForBg] = '\0';
            daemon = 1;
        }
        
        pid_t pid;
        pid = fork();
        
        
        if (pid == 0){
            
            executeCommandfromChild(cmdl);
            exit(1);
            
        }
        else if(pid > 0)
        {
            if(daemon){
                
                struct daemonCom* backGroundProcess= malloc(sizeof(struct daemonCom));
                backGroundProcess->pid = pid;
                backGroundProcess->cmd = (char*)malloc(512);
                strcpy(backGroundProcess->cmd, cmdl);
                int status;
                if(waitpid(getBC(backGroundProcessList)->pid,&status, WNOHANG)<0){
                    fprintf(stderr, "+ completed '%s' [%d]\n", getBC(backGroundProcessList)->cmd, WEXITSTATUS(status));
                }
            }
            
            if(daemon){
                int status;
                if(waitpid(getBC(backGroundProcessList)->pid,&status, WNOHANG)<0){
                    fprintf(stderr, "+ completed '%s' [%d]\n", getBC(backGroundProcessList)->cmd, WEXITSTATUS(status));
                }
            }else{
                waitpid(pid, &status, 0);
                fprintf(stderr, "+ completed '%s' [%d]\n", cmdl, WEXITSTATUS(status));
            }
        }
        else
        {
            perror("fork");
            exit(1);
        }
    }
    //return EXIT_SUCCESS;
}
