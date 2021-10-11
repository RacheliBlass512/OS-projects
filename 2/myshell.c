#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
void sigchild_handler()
{
    int status;
    wait(&status);
    return;
}

int prepare(void)
{
    //the shell
    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    return 0;
}

 int finalize(void) {
	int stat,status;
	do{
		status=wait(&stat);
		}while(status!=-1);
	return 0;
}

void sigint_handler_kill()
{
    //kill the process
    pid_t current_pid = getpid();
    kill(current_pid, SIGKILL);
}
int process_arglist(int count, char **arglist)
{
    pid_t pid1, pid2;
    int status;
    int index;
    int option = 1;
    int pipefd[2];
    //Checks which option the command is in
    if (!strcmp(arglist[count - 1], "&"))
    {
        option = 2;
    }
    else
    {
        for (index = 0; index < count; index++)
        {
            if (!strcmp(arglist[index], "|"))
            {
                option = 3;
                break;
            }
        }
    }
    //Execution of the command
    switch (option)
    {
    //Regular command
    case 1:
        pid1 = fork();
        if (pid1 == -1)
        {
            fprintf(stderr, "Failed to fork");
            exit(1);
        }
        if (pid1 == 0)
        {
            //son
            signal(SIGINT, &sigint_handler_kill);
            if (execvp(arglist[0], arglist) == -1)
            {
                fprintf(stderr, "execvp Failed");
                exit(1);
            }
        }
        if (pid1 > 0)
        {
            //father
            waitpid(pid1, &status, 0);
        }
        break;
    //In the background
    case 2:
        pid1 = fork();
        if (pid1 == -1)
        {
            fprintf(stderr, "Failed to fork");
            exit(1);
        }
        if (pid1 == 0)
        {
            //son
            arglist[count - 1] = NULL;
            signal(SIGCHLD, &sigchild_handler);
            if (execvp(arglist[0], arglist) == -1)
            {
                fprintf(stderr, "execvp Failed");
                exit(1);
            }
        }
        break;
    //Command with Pipe
    case 3:
        if (pipe(pipefd) == -1)
        {
            fprintf(stderr, "pipe");
            exit(1);
        }
        pid1 = fork();
        if (pid1 == -1)
        {
            fprintf(stderr, "Failed to fork");
            exit(1);
        }
        if (pid1 == 0)
        {
            //first son
            arglist[index] = NULL;
            signal(SIGINT, &sigint_handler_kill);
            close(1);
            dup2(pipefd[1], 1);
            if (execvp(arglist[0], arglist) == -1)
            {
                fprintf(stderr, "execvp Failed");
                exit(1);
            }
        }
        else
        {
            //father
            close(pipefd[1]);
            pid2 = fork();
            if (pid2 == -1)
            {
                fprintf(stderr, "Failed to fork");
                exit(1);
            }
            if (pid2 == 0)
            {
                //secend son
                close(0);
                dup2(pipefd[0], 0);
                if (execvp(arglist[index + 1], arglist + index + 1) == -1)
                {
                    fprintf(stderr, "execvp Failed");
                    exit(1);
                }
            }
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
        }
        break;
    default:
        break;
    }
    return 1; 
}