#General Information
*ECS150 Project 1:* Simple Shell
*Authors:* Jisoo Yun, Kevin Chung

###Phase 0
We created a makefile with the specified *-Wall* and *-Werror* warning options.
Also, we added a simple clean rule.

###Phase 1
First, we started with a rudimentary *fork+exec+wait* method. Using the *fork()*
system call, we create a new process which is a child process copy of the parent
process. To allow for the child process to run, we call *waitpid()* to let the 
parent wait for the child process to terminate. *exec()* is called to make the 
child process execute after forking.

###Phase 2
For this part, we used a while loop to print out the *$sshell* prompt and get
the line with *fgets()*. After looking through some documentation, we decided to
use *execvp* to execute the commands. In order to get the correct exit status 
"1", we used *WEXITSTATUS(status)* to get the correct value when outputting the 
*complete* statement. Trying to get the *execvp()* syntax to be correct with 
the proper arguments was the main difficulty of this phase.

###Phase 3
To address the added functionality of introducing arguments in the user input,
we primarily parsed the user input by the tokenizing via spaces. We created a 
char pointer array to hold each token using a while loop and *strtok()*. 
Unfortunately, we did not use a struct for this phase. Everything was stored in
char pointer arrays.

###Phase 4
The built-in commands were separated into different functions: *exit_command()*,
*pwd_command()*, and *cd_command()*. Both *exit_command()* and *pwd_command()*
take in a char* command and use *strcmp()* to check if the user input has "exit"
or "pwd" and print out the respective messages. For our *cd* implementation, we
introduced the parsed char double pointer: *cmdWithArg* to check whether the 
argument was ".." or not. If it is ".." we just called chdir(".."), otherwise we
used *strcat()* to append the new path onto the current path. Of course, we made
sure to check if the directory is valid in the first place and printing an error 
if the directory is unrecognizable. One minor annoyance in this phase was that 
pwd seemed to work by default without any implementation. So when we used our 
function, there were duplicate outputs. To remedy this we included a conditional
before calling *execvp()* to *exit(0)* if the user input is pwd.

###Phase 5


###Phase 6


###Phase 7
This part started out with the parsing of the user input into individual 
commands separated by the *|* character. So we tokenized with a similar process
to *phase 3* and then passed each tokenized string into our *parseThenExecute()*
function to parse each string further according to whitespace. 

###Phase 8


