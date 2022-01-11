/* See Chapter 5 of Advanced UNIX Programming:  http://www.basepath.com/aup/
 *   for further related examples of systems programming.  (That home page
 *   has pointers to download this chapter free.
 *
 * Copyright (c) Gene Cooperman, 2006; May be freely copied as long as this
 *   copyright notice remains.  There is no warranty.
 */

/* To know which "includes" to ask for, do 'man' on each system call used.
 * For example, "man fork" (or "man 2 fork" or man -s 2 fork") requires:
 *   <sys/types.h> and <unistd.h>
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAXLINE 200  /* This is how we declare constants in C */
#define MAXARGS 20
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>    /* 'man fprintf' says we need this. */
#include <sys/wait.h> /* 'man waitpid' says we need this. */

#define PIPE_READ 0
#define PIPE_WRITE 1
int flag = 0;
/* In C, "static" means not visible outside of file.  This is different
 * from the usage of "static" in Java.
 * Note that end_ptr is an output parameter.
 */
static char * getword(char * begin, char **end_ptr) {
    char * end = begin;

    while ( *begin == ' ' )
        begin++;  /* Get rid of leading spaces. */
    end = begin;
    while ( *end != '\0' && *end != '\n' && *end != ' ' )
        end++;  /* Keep going. */
    if ( end == begin )
        return NULL;  /* if no more words, return NULL */
    *end = '\0';  /* else put string terminator at end of this word. */
    *end_ptr = end;
    if (begin[0] == '$') { /* if this is a variable to be expanded */
        begin = getenv(begin+1); /* begin+1, to skip past '$' */
	if (begin == NULL) {
	    perror("getenv");
	    begin = "UNDEFINED";
        }
    }
    return begin; /* This word is now a null-terminated string.  return it. */
}

/* In C, "int" is used instead of "bool", and "0" means false, any
 * non-zero number (traditionally "1") means true.
 */
/* argc is _count_ of args (*argcp == argc); argv is array of arg _values_*/
static void getargs(char cmd[], int *argcp, char *argv[])
{
    char *cmdp = cmd;
    char *end;
    int i = 0;
    /*if 0, continue wile loop. If 1, store the current word as inputFile, 2 as outputFile*/ 
    /* fgets creates null-terminated string. stdin is pre-defined C constant
     *   for standard intput.  feof(stdin) tests for file:end-of-file.
     */
   if (fgets(cmd, MAXLINE, stdin) == NULL && feof(stdin)) {
        printf("Couldn't read from standard input. End of file? Exiting ...\n");
        exit(1);  /* any non-zero value for exit means failure. */
    }
    while ((cmdp = getword(cmdp, &end)) != NULL) { /* end is output param */
	if (*cmdp == '#') {
	   break;
	}
	if (*cmdp == '>') {
	   flag = 1;
	}
	if (*cmdp == '<') {
	   flag = 2;
	}
	if (*cmdp == '|') {
	   flag = 3;
	}      	   	   
        /* getword converts word into null-terminated string */        
        argv[i++] = cmdp;  
	/*printf("%d \n", argv[--i]);*/
        /* "end" brings us only to the '\0' at end of string */
	cmdp = end + 1;
	
    }
       
    argv[i] = NULL; /* Create additional null word at end for safety. */
    *argcp = i;
}

int pipeHelper(int argc, char *argv[]) {
    int pipe_fd[2];       /* 'man pipe' says its arg is this type */
    int fd;               /* 'man dup' says its arg is this type */
    pid_t child1, child2; /* 'man fork' says it returns type 'pid_t' */
    char * argvChild[2];


    if (-1 == pipe(pipe_fd)) {
      perror("pipe");
    }

    child1 = fork();
    /* child1 > 0 implies that we're still the parent. */
    if (child1 > 0) {
      child2 = fork();
    }

    if (child1 == 0) { /* if we are child1, do:  "ls | ..." */
        /* close  */
        if (-1 == close(STDOUT_FILENO)) {
          perror("close");
        }

        fd = dup(pipe_fd[PIPE_WRITE]); /* set up empty STDOUT to be pipe_fd[1] */
        if (-1 == fd) {
            perror("dup");
        }

        if (fd != STDOUT_FILENO) {
            fprintf(stderr, "Pipe output not at STDOUT.\n");
        }

        close(pipe_fd[PIPE_READ]); /* never used by child1 */
        close(pipe_fd[PIPE_WRITE]); /* not needed any more */

        argvChild[0] = argv[0];
        argvChild[1] = NULL;

        if (-1 == execvp(argvChild[0], argvChild)) {
            perror("execvp");
        }

    } else if (child2 == 0) { /* if we are child2, do:  "... | wc" */

        /* close  */
        if (-1 == close(STDIN_FILENO)) {
            perror("close");
        }

        /* set up empty STDIN to be pipe_fd[0] */
        fd = dup(pipe_fd[PIPE_READ]);
        if (-1 == fd) {
            perror("dup");
        }

        if (fd != STDIN_FILENO) {
            fprintf(stderr, "Pipe input not at STDIN.\n");
        }

        close(pipe_fd[PIPE_READ]); /* not needed any more */
        close(pipe_fd[PIPE_WRITE]); /* never used by child2 */

        argvChild[0] = argv[2];
        argvChild[1] = NULL;

        if (-1 == execvp(argvChild[0], argvChild)) {
            perror("execvp");
        }

    } else { /* else we're parent */
        int status;
        /* Close parent copy of pipes;
 *          * In particular, if pipe_fd[1] not closed, child2 will hang
 *                   *   forever waiting since parent could also write to pipe_fd[1]
 *                            */
        close(pipe_fd[PIPE_READ]);
        close(pipe_fd[PIPE_WRITE]);

        if (-1 == waitpid(child1, &status, 0)) {
            perror("waitpid");
        }

        /* Optionally, check return status.  This is what main() returns. */
        if (WIFEXITED(status) == 0) {
            printf("child1 returned w/ error code %d\n", WEXITSTATUS(status));
        }

        if (-1 == waitpid(child2, &status, 0)) {
            perror("waitpid");
        }

        /* Optionally, check return status.  This is what main() returns. */
        if (WIFEXITED(status) == 0) {
            printf("child2 returned w/ error code %d\n", WEXITSTATUS(status));
        }
    }
    return 0;  /* returning 0 from main() means success. */
}

static void execute(int argc, char *argv[]) /* Special char is 0 if none of < > | or & are provided, or 1 2 3 4 if they are, respectively */
{
    pid_t childpid; /* child process ID */
    int background = 0; /* 0 if the & is not present, 1 if it is*/
    childpid = fork();

    if (strcmp(argv[argc - 1],"&") == 0) { /*checks if the & is the last input*/
	argv[argc-1] = NULL;
	argc--;
	background = 1;
    }
    if (flag == 1) {/*FOR OUTPUT REDIRECTION*/
	close(1);
	int fd = open("output_file", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) perror("open for writing");
	argv[argc - 2] = NULL;
	argv[argc - 1] = NULL;
	argc -= 2;
	flag = 0;
    }
    if (flag == 2) {/*FOR INPUT REDIRECTION*/
	close(0);
	int fd = open("input_file", O_RDONLY);  /* Now any read from stdin reads from input_file. */
  	if (fd == -1) perror("open for reading");
	argv[argc - 2] = NULL;
	argv[argc - 1] = NULL;
	argc -= 2;
	flag = 0;
    }
    
    if (childpid == -1) { /* in parent (returned error) */
        perror("fork"); /* perror => print error string of last system call */
        printf("  (failed to execute command)\n");
    }
    if (childpid == 0) { /* child:  in child, childpid was set to 0 */
        /* Executes command in argv[0];  It searches for that file in
	 *  the directories specified by the environment variable PATH.
         */

        
	/*FOR INPUT REDIRECT < */
 	/*close(STDIN_FILENO); */
  	/* This causes any input from stdin to comre from input_file: see 'man open' */
  	/*fd = open("input_file", O_RDONLY);  Now any read from stdin reads from input_file. */
  	/* if (fd == -1) perror("open for reading"); */

        if (-1 == execvp(argv[0], argv)) {
          perror("execvp");
          printf("  (couldn't find command)\n");
        }
	/* NOT REACHED unless error occurred */
        exit(1);
    } else {/* parent:  in parent, childpid was set to pid of child process */
	if (background == 0) {
        	waitpid(childpid, NULL, 0);  /* wait until child process finishes */
    	}
	}    
    return;
}
void interrupt_handler(int signum){
	printf("Interrupt handler recieved, killing command!\n");
}

int main(int argc, char *argv[])
{
    char cmd[MAXLINE];
    char *childargv[MAXARGS];
    int childargc;
    signal(SIGINT, interrupt_handler);
    if (argc > 1) {
	freopen(argv[1], "r", stdin);
    }
    while (1) {
        printf("%% "); /* printf uses %d, %s, %x, etc.  See 'man 3 printf' */
        fflush(stdout); /* flush from output buffer to terminal itself */
	getargs(cmd, &childargc, childargv); /* childargc and childargv are
            output args; on input they have garbage, but getargs sets them. */
        /* Check first for built-in commands. */
	if ( childargc > 0 && strcmp(childargv[0], "exit") == 0 )
            exit(0);
	else if ( childargc > 0 && strcmp(childargv[0], "logout") == 0 )
            exit(0);
	else if (flag == 3) {
	    pipeHelper(childargc, childargv);
	    flag = 0;
	}
        else
	    execute(childargc, childargv);
    }
    /* NOT REACHED */
}
