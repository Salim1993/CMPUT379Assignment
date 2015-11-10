#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

/* This program forks and and the prints whether the process is
 *   - the child (the return value of fork() is 0), or
 *   - the parent (the return value of fork() is not zero)
 *
 * When this was run 100 times on the computer the author is
 * on, only twice did the parent process execute before the
 * child process executed.
 *
 * Note, if you juxtapose two strings, the compiler automatically
 * concatenates the two, e.g., "Hello " "world!"
 */

 //https://ece.uwaterloo.ca/~dwharder/icsrts/Tutorials/fork_exec/

 struct Pipes
{
    //int rdwr[2];   // 0 = read, 1 = write
	int rd[2];		//child will read 
	int wr[2];		//child will write
};

struct process
{
   int  processID;
   char  name[256];
   int   time;
};

struct child{
	int childPID;
	int status;
	//might need address to pipe;
	struct process proc;
};

struct pipeMessage{
	char type[4]; // This is the type of message
	//NEW for new child to manage pid (parent to child)
	//OLD for new child to process (child to parent)
	//KIL child killed process (child to parent)
	//GON child didnt kill process (child to parent)
	struct process procMessage;
};


int main( void ) {

	pid_t parentPID = getpid();
	int i;
	int busyChildArray[128];
	int idleChildArray[128];
	int counter = 3;
	int	fd[256][2];
	char line[256];
	char readline[256];

	struct Pipes pipeList[256];

	for(i=0;i < counter; i++){
		pipe(pipeList[i].rd);
		pipe2(pipeList[i].wr,O_NONBLOCK);
		//pipe(fd[i]);		
		int writestatus = write(pipeList[i].rd[1], "lolol",6);
		

		int pid = fork();
		if(pid < 0){
	    	//err_sys("fork error");
	    }
		else if ( pid == 0 ) {  //child

			
			close(pipeList[i].rd[1]);
			int k = read(pipeList[i].rd[0], readline, 256);
			printf("(%d) THe child has recieved the line from parent: %s\n",k,readline);
			close(pipeList[i].rd);
			

			char *tempHolder = malloc(6);
			sprintf(tempHolder, "%d", getpid());
			close(pipeList[i].wr[0]);
			int readstatus = write(pipeList[i].wr[1], tempHolder, 6);
			//close(fd[i][0]);
			//int status = write(fd[i][1], tempHolder, 6);

			printf("The child %d has attempted write (%d). \n",getpid(),readstatus);
			free(tempHolder);
			exit(EXIT_SUCCESS);
		} else {  //parent
			busyChildArray[i] = pid;
			printf("this is the iteration %d and the pid is %d\n",i,pid );
		}
	}
	int j = 0;
	int n;
	while(j < counter){
		n = read(pipeList[j].wr[0], line, 256);
		if (n > 1){
			//close(fd[j][1]);
			close(pipeList[j].wr[1]);
			//int n = read(fd[j][0], line, 256);
			int propPID = atoi(line);
			printf("(%d)   child returned its pid which is %s\n",n, line);
			close(pipeList[j].wr);
			j++;
		}
	}
	return 0;
}