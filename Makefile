all: sshell

sshell: sshell.c
	gcc -Wall -Werror -g -o sshell sshell.c

clean:
	rm sshell
