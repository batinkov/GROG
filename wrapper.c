// gcc -std=c99 -pedantic -Wall -Werror -O2 -o wrapper wrapper.c

#define _XOPEN_SOURCE 600
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

const useconds_t childSleepTimeMs = 500000;
static sig_atomic_t terminateChild = 0;

void HandeSIGTERM(int signum);
int HandleChild(pid_t childPid);


int main( int argc, char *argv[] )
{
    if(2 > argc)
    {
        fprintf(stderr, "At least one argument must be specified! \n");
        exit(1);
    }

    int masterFileDescriptor = posix_openpt( O_RDWR | O_NOCTTY );
    char *slaveDeviceName = NULL; // get the name of the slave pseudoterminal

    if(-1 == masterFileDescriptor)
    {
        perror("Unable to open a pseudoterminal master device!");
        exit(20);
    }
    else if(0 != grantpt(masterFileDescriptor))
    {
        perror("Unable to grant access to the slave pseudo-terminal!");
        exit(21);
    }
    else if(0 != unlockpt(masterFileDescriptor))
    {
        perror( "Unable to unlock a pseudo-terminal master/slave pair!" );
        exit(22);
    }
    else if(NULL == (slaveDeviceName = ptsname(masterFileDescriptor)))
    {
        perror("Unable to get the name of the slave pseudo-terminal!");
        exit(23);
    }

    pid_t childPid = fork(); // Try to create a child process
    if(0 < childPid)
    { // This is the parrent process
        signal(SIGTERM, HandeSIGTERM); // catch SIGTERM

        // Make stdout unbuffered
        setvbuf(stdout, (char *)NULL, _IONBF, 0);

        FILE *masterFilePointer;
        // Try to open the master end of the pty
        if(NULL == (masterFilePointer = fdopen( masterFileDescriptor, "r" )))
        {
            terminateChild = 1;
            HandleChild(childPid);
            perror("Unable to open master end of the pty!");
            exit(3);
        }

        int ch;
        while(EOF != (ch = fgetc(masterFilePointer)))
        {
            fputc(ch, stdout);
        }

        int childExitStatus = HandleChild(childPid);

        close(masterFileDescriptor);

        // Returns the exit status of the child
        return WEXITSTATUS(childExitStatus);
    }
    else if(0 == childPid)
    { // This is the child process
        int slaveFileDescriptor;

        usleep(childSleepTimeMs);

        // Try to open the slave end of the pty
        if(-1 == (slaveFileDescriptor = open(slaveDeviceName, O_RDWR|O_NOCTTY)))
        {
            perror("Unable to open a slave end of the pty!");
            exit(4);
        }

        dup2(slaveFileDescriptor, STDOUT_FILENO);
        dup2(slaveFileDescriptor, STDERR_FILENO);

        close(masterFileDescriptor);
        close(slaveFileDescriptor);

        execvp(argv[1], &argv[1]);

        perror("Exec failed!");

        return 1;
    }
    else
    { // ERROR occured while forking
        close(masterFileDescriptor);
        perror("No child process is created!");
        exit(3);
    }
}

void HandeSIGTERM(int signum)
{
    terminateChild = 1;
}

int HandleChild(pid_t childPid)
{
    int childExitStatus = 0;
    int childStatus = -1;

    do
    {
        if(1 == terminateChild)
        {
            kill(childPid, SIGTERM);
            // TODO: maybe reset terminateChild
        }

        waitpid(childPid, &childExitStatus, WNOHANG);

        childStatus = kill(childPid, 0); // get the status of the child

        sleep(2);
    } while(-1 != childStatus);

    return childExitStatus;
}
